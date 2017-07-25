/*
 * File: sws.c
 * Author: Alex Brodsky
 * Purpose: This file contains the implementation of a simple web server.
 *          It consists of two functions: main() which contains the main
 *          loop accept client connections, and serve_client(), which
 *          processes each client request.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "network.h"
#include "sws.h"
#include "scheduler.h"


/* -------------------------------------------------------------------------- *
 * Purpose: This function takes a file handle to a client, reads in the request,
 *    parses the request, and sends back the requested file.  If the
 *    request is improper or the file is not available, the appropriate
 *    error is sent back.
 * Parameters:
 *             fd : the file descriptor to the client connection
 * Returns: None
 * -------------------------------------------------------------------------- */
pthread_mutex_t mutex;
int sequence_number;
enum scheduler_type scheduler;

void safe_enqueue(struct RCB *new, int admit)
{
    pthread_mutex_lock(&mutex);
    if (admit == 1) admit_scheduler(new);
    else resubmit_scheduler(new);
    pthread_mutex_unlock(&mutex);
}
struct RCB *safe_dequeue()
{
    pthread_mutex_lock(&mutex);
    struct RCB *new = get_next_scheduler();
    pthread_mutex_unlock(&mutex);
    return new;
}


static void serve_client( int fd )
{
    //pthread_mutex_lock(&mutex);
    static char *buffer;                            /* Request buffer         */
    char *req = NULL;                               /* ptr to req file        */
    char *brk;                                      /* state used by strtok   */
    char *tmp;                                      /* error checking ptr     */
    FILE *fin;                                      /* input file handle      */
    int len;                                        /* length of data read    */
    long int fileSize;                              /* Size of requested file */
    struct RCB *request;                            /* Request control block  */

    /* 1st time, alloc buffer */
    if ( !buffer )
    {
        buffer = malloc( MAX_HTTP_SIZE );
        /* error check */
        if ( !buffer )
        {
            perror( "Error while allocating memory" );
            abort();
        }
    }


    memset( buffer, 0, MAX_HTTP_SIZE );
    if ( read( fd, buffer, MAX_HTTP_SIZE ) <= 0 )    /* read req from client  */
    {
        perror( "Error while reading request" );
        abort();
    }

    /* standard requests are of the form
     *   GET /foo/bar/qux.html HTTP/1.1
     * We want the second token (the file path).
     */
    tmp = strtok_r( buffer, " ", &brk );              /* parse request */
    if ( tmp && !strcmp( "GET", tmp ) )
    {
        req = strtok_r( NULL, " ", &brk );
    }

    if ( !req )                                       /* is req valid? */
    {
        len = sprintf( buffer, "HTTP/1.1 400 Bad request\n\n" );
        write( fd, buffer, len );                     /* if not, send err */
    }
    else                                              /* if so, open file */
    {
        req++;                                        /* skip leading /   */
        fin = fopen( req, "r" );
        /* open file */
        if ( !fin )                                    /* check if successful */
        {
            len = sprintf( buffer, "HTTP/1.1 404 File not found\n\n" );
            write( fd, buffer, len );                  /* if not, send err    */
        }
        else                                           /* if so, send file    */
        {
            /* send success code */
            len = sprintf( buffer, "HTTP/1.1 200 OK\n\n" );
            write( fd, buffer, len );
            request = malloc(sizeof(struct RCB));     /* Create RCB           */
            request->fd = fd;                         /* Client ID            */
            request->fptr = fin;                      /* Requested File       */
            fseek(fin, 0, SEEK_END);
            fileSize = ftell(fin);
            fseek(fin, 0, SEEK_SET);
            printf("The file size is: %ld bytes", fileSize);
            request->remainbytes = fileSize;          /* initially = filesize */
            request->seq = sequence_number++;         /* Request Sequence     */
            switch (scheduler)
            {
            /* SJF --> send entire file */
            case SJF:
                request->quantum = fileSize;
                break;

            /*   MLFQ & RR --> send only 8KB initially */
            case RR:
            case MLFQ:
                request->quantum = HIGH_PRIORITY_QUANTUM;
                break;
            }
            safe_enqueue(request, 1);
            //admit_scheduler(request);
        }
    }
}

/* -------------------------------------------------------------------------- *
 * Purpose: Serve a single request from the queue. Resubmit to queue if needed.
 *
 * Parameters: none
 * Returns: integer denoting if the queue is empty or not,
 * -------------------------------------------------------------------------- */
int serve_request()
{
    long len;                                    /* length of segment         */
    static char *buffer;                         /* Request buffer            */
    struct RCB *req = safe_dequeue();
    //struct RCB *req = get_next_scheduler();      /* Get next request          */
    if (req == NULL)                             /* IF no request are pending */
        return FALSE;                            /* */
    long left = req->remainbytes;                /* bytes left to send        */
    long quantum = req->quantum;                 /* bytes per each serve      */

    /* Queue is empty */

    /* 1st time, alloc buffer */
    if ( !buffer )
    {
        buffer = malloc( MAX_HTTP_SIZE );
        /* error check */
        if ( !buffer )
        {
            perror( "Error while allocating memory" );
            abort();
        }
    }
    memset( buffer, 0, MAX_HTTP_SIZE );

    do
    {
        if ( left > MAX_HTTP_SIZE)
            left = MAX_HTTP_SIZE;
        len = fread( buffer, 1, left, req->fptr );         /* read file chunk */
        if ( len < 0 )                                     /* check for errors*/
        {
            perror( "Error while writing to client" );
        }
        else if ( len > 0 )                                /* if none,send req*/
        {
            len = write( req->fd, buffer, len );
            printf("%s", buffer);
            fflush(stdout);
            if ( len < 1 )                                 /* check for errors*/
            {
                perror( "Error while writing to client" );
            }
            req->remainbytes -= left;
            quantum -= left;
            if (scheduler == SJF && quantum > 0)
                left = quantum;
        }

    }
    while (len == MAX_HTTP_SIZE && quantum > 0);

    /* IF request is fully serviced */
    if (req->remainbytes == 0)
    {
        fclose( req->fptr );                       /* Close requested file    */
        close(req->fd);                            /* Close client descriptor */
        free(req);
    }
    else
        safe_enqueue(req, 0);
        //resubmit_scheduler(req);

    return TRUE;               /* Assume queue not empty, recheck upon recall */
}

/*
 * entry function for the threads
 */
void *entry(void *rcb)
{
    for (;;)
    {
        serve_request();
    }
}


/* -------------------------------------------------------------------------- *
 * Purpose: initialize threads
 * Parameters:
 * Returns:
 * -------------------------------------------------------------------------- */
void thread_init(int numThreads, int port)
{
    int i;
    pthread_t threads[numThreads];
    for (i = 0; i < numThreads; i++)
    {
        pthread_create(&threads[i], NULL, entry, NULL);
    }
    get_reqs(port);
    for (i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}


/*
 * waits for requests, then schedules them
 */
int get_reqs(int port)
{
    int fd;
    sequence_number = 1;
    network_init( port );                              // init network module

    for ( ;; )                                         // infinite main loop
    {
        network_wait();                                // wait for clients

        while ((fd = network_open()) >= 0)
        {
            if (fd >= 0)
                serve_client( fd );                    // process each client

            //queue_status = serve_request();            // Process requests
        }
    }
}

/* -------------------------------------------------------------------------- *
 * Purpose: This function is where the program starts running.
 *    The function first parses its command line parameters to determine port #
 *    Then, it initializes, the network and enters the main loop.
 *    The main loop waits for a client (1 or more to connect, and then processes
 *    all clients by calling the seve_client() function for each one.
 * Parameters:
 *             argc : number of command line parameters (including program name
 *             argv : array of pointers to command line parameters
 * Returns: an integer status code, 0 for success, something else for error.
 * -------------------------------------------------------------------------- */
int main( int argc, char **argv )
{
    int port;                                       /* Server port #          */
    //int fd;                                         /* Client file descriptor */
    int queue_status = 0;                           /* Pending queue status   */

    if ( argc < 4 )                                 /* Check for parameters   */
    {
        printf( "usage: sws <port> <scheduler> <threads>\n" );
        return 0;
    }

    sscanf( argv[1], "%d", &port );                 /* Get port number        */

    /* Check if port is within specified range */
    if (port < 1024 || port > 65535)
    {
        printf( "Inaccessible port\n" );
        return 0;
    }

    /* Check if Scheduler is supported */
    if ((scheduler = scheduler_init(argv[2])) == BAD_SCHEDULER)
    {
        printf( "Unsupported Scheduler\n" );
        return 0;
    }
    thread_init(sscanf(argv[3], "%d"), port);                              /* Initialize threads  */
    /*
    sequence_number = 1;
    network_init( port );                              // init network module

    for ( ;; )                                         // infinite main loop
    {
        network_wait();                                // wait for clients

        while ((fd = network_open()) >= 0 || queue_status)
        {
            if (fd >= 0)
                serve_client( fd );                    // process each client

            queue_status = serve_request();            // Process requests
        }
    }
    */
}
