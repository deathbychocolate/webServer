
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
#include "queue.h"

int sequence_number;                        /* Global sequence number */
enum scheduler_type scheduler;              /* Scheduler type */
struct work_queue *queue;                   /* Thread work queue */

/* Mutual exclusion locks */
pthread_mutex_t lock_rcb = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_client = PTHREAD_MUTEX_INITIALIZER;

/* Condition monitor */
pthread_cond_t cond_rcb  = PTHREAD_COND_INITIALIZER;

/* Counter to ensure maximum RCBs do not exceed MAX_REQS (defined as 100) */
int req_tbl_cntr;


/* -------------------------------------------------------------------------- *
 * Purpose: This function takes a file handle to a client, reads in the request,
 *    parses the request, and sends back the requested file.  If the
 *    request is improper or the file is not available, the appropriate
 *    error is sent back.
 * Parameters:
 *             fd : the file descriptor to the client connection
 * Returns: None
 * -------------------------------------------------------------------------- */

static void serve_client( struct RCB *request )
{
    static char *buffer;                            /* Request buffer         */
    char *req = NULL;                               /* ptr to req file        */
    char *brk;                                      /* state used by strtok   */
    char *tmp;                                      /* error checking ptr     */
    FILE *fin;                                      /* input file handle      */
    int len;                                        /* length of data read    */
    long int fileSize;                              /* Size of requested file */
    
    pthread_mutex_lock(&lock_client);               /* Lock client            */
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
    
    /* read req from client  */
    if ( read( request->fd, buffer, MAX_HTTP_SIZE ) <= 0 )
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
        req = strtok_r( NULL, " ", &brk );

    if ( !req )                                       /* is req valid? */
    {
        len = sprintf( buffer, "HTTP/1.1 400 Bad request\n\n" );
        write( request->fd, buffer, len );            /* if not, send err */
    }
    else                                              /* if so, open file */
    {
        req++;                                        /* skip leading /   */
        fin = fopen( req, "r" );
        /* open file */
        if ( !fin )                                    /* check if successful */
        {
            len = sprintf( buffer, "HTTP/1.1 404 File not found\n\n" );
            write( request->fd, buffer, len );         /* if not, send err    */
        }
        else                                           /* if so, send file    */
        {
            /* send success code */
            strncpy( request->path, req, FILENAME_MAX);/* Get file name       */
            
            len = sprintf( buffer, "HTTP/1.1 200 OK\n\n" );
            write( request->fd, buffer, len );
            request->fptr = fin;                      /* Requested File       */
            fseek(fin, 0, SEEK_END);
            fileSize = ftell(fin);                    /* Get file size        */
            fseek(fin, 0, SEEK_SET);
            request->remainbytes = fileSize;          /* initially = filesize */
            switch (scheduler)
            {
            /*   SJF --> send entire file */
            case SJF:
                request->quantum = fileSize;
                break;

            /*   MLFQ & RR --> send only 8KB initially */
            case RR:
            case MLFQ:
                request->quantum = HIGH_PRIORITY_QUANTUM;
                break;
            }
        }
    }
    pthread_mutex_unlock(&lock_client);               /* Unlock client        */
}

/* -------------------------------------------------------------------------- *
 * Purpose: Serve a single request from the queue until its quantum is processed
 *              or until the the file is done. whichever is first.
 * Parameters: none
 * Returns: integer denoting if request needs resubmission
 * -------------------------------------------------------------------------- */
int serve_request(struct RCB *req)
{
    long len;                                    /* length of segment         */
    static char *buffer;                         /* Request buffer            */
    if (req == NULL)                             /* req does not need resub   */
        return FALSE;                            /* return if RCB is not found*/
    long left = req->remainbytes;                /* bytes left to send        */
    long quantum = req->quantum;                 /* bytes per each serve      */
    
    /* Check if file size is larger than allowed quantum */
    if (left > quantum)
        req -> sent = quantum;                   /* Bytes sent is the quantum */
    else
        req -> sent = left;                      /* all remaining bytes sent  */
    
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

    /* loop while sending chunks of 8KB each to client up until quantum limit 
       or until the request is complete whichever comes first */
    do
    {
        if ( left > MAX_HTTP_SIZE)         /* If what is left is less than 8KB*/
            left = MAX_HTTP_SIZE;
        len = fread( buffer, 1, left, req->fptr );         /* read file chunk */
        if ( len < 0 )                                     /* check for errors*/
        {
            perror( "Error while writing to client" );
        }
        else if ( len > 0 )                                /* if none,send req*/
        {
            len = write( req->fd, buffer, len );
            fflush(stdout);
            if ( len < 1 )                                 /* check for errors*/
            {
                perror( "Error while writing to client" );
            }
            req->remainbytes -= left;             /* Remaining bytes updated  */
            quantum -= left;                      /* Remaining quantum updated*/
            
            /* If quantum is larger than 8KB, ensure quantum is fully written */
            if ((scheduler == SJF || scheduler == MLFQ) && quantum > 0)
                left = quantum;
        }
    }
    while (len == MAX_HTTP_SIZE && quantum > 0);

    /* IF request is fully serviced */
    if (req->remainbytes == 0)
        return FALSE;          /* Request completed. no resubmission required */
    else
        return TRUE;           /* request incomplete. resubmission is needed  */
}

/* -------------------------------------------------------------------------- *
 * Purpose: worker thread function where all worker threads enter an infinite 
 *              loop where the threads check the work queue for request to admit
 *              to the scheduler as well as serving these requests. A thread 
 *              sleeps only if worker and scheduler queue's are empty
 * Parameters: none
 * Returns: none
 * -------------------------------------------------------------------------- */
static void *worker_queue( void * arg ) {
    struct RCB *req;
    int wait = 1;
    int status = 0;
    while (1)                                      /* Infinite thread loop    */
    {
        req = dequeue(queue, wait);                /* Aquire from work queue  */
        if( req )
        {
            serve_client( req );                       /* process client req  */
            if (req ->fptr !=NULL)                     /* IF file found       */
            {
                safe_enqueue(req, 1);                  /* Admit to scheduler  */
                printf( "Request for file %s admitted.\n", req->path );
                fflush( stdout );
            }
            else                                       /* ELSE file not found */
            {
                close( req->fd );                      /* Close connection    */
                pthread_mutex_lock( &lock_rcb );       /* Lock RCB freeing    */
                free(req);                             /* Free RCB            */
                req_tbl_cntr--;                        /* Request table count */
                pthread_cond_signal( &cond_rcb );      /* Signal main thread  */
                pthread_mutex_unlock( &lock_rcb );     /* Unlock queue        */
            }
        }
        else    /* no RCB in work queue but present in scheduler queue */
        {
            req = safe_dequeue();                      /* Get RCB from queue  */
            status = serve_request(req);               /* Serve client request*/
            
            /* IF request is present and RCB needs resubmission to scheduler  */
            if( req && status )
            {
                printf( "Sent %ld bytes of file %s.\n", req->sent, req->path );
                fflush( stdout );
                safe_enqueue(req,0);                   /* Resubmit request    */
            }
            else if( req )                             /* Else Req completed  */
            {
                printf( "Sent %ld bytes of file %s.\n", req->sent, req->path );
                printf( "Request for file %s completed.\n", req->path );
                fflush( stdout );
                close( req->fd );                      /* Close connection    */
                fclose(req->fptr);                     /* Close client file   */
                
                /* Free RCB */
                pthread_mutex_lock( &lock_rcb );
                free(req);
                req_tbl_cntr--;
                pthread_cond_signal( &cond_rcb );
                pthread_mutex_unlock( &lock_rcb );
            }
        }
        /* If request is done, suspend thread if work queue empty*/
        if (req == NULL)
            wait = 1;
        /* Else thread does not suspend in order to continue serving RCB */
        else
            wait = 0;
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
    int threads;                                    /* Number of threads      */
    int fd;                                         /* Client file descriptor */
    struct RCB *request;                            /* Client requests        */
    
    /* Check for parameters */
    if ( argc < 4 || ( sscanf( argv[1], "%d", &port    ) < 1 )
                  || ( sscanf( argv[3], "%d", &threads ) < 1 )) {
        printf( "usage: sws <port> <scheduler> <threads>\n" );
        return 0;
    }

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

    /* Check if number of threads is positive */
    if (threads < 1)
    {
        printf( "Number of threads has to be larger than 1\n" );
        return 0;
    }
    
    /* initialize the threads work queue */
    queue = (struct work_queue*) malloc (sizeof(struct work_queue));
    
    /* Instantiate and start the specified number of worker threads */
    pthread_t thread_array[threads];
    for (int i = 0; i < threads; i++)
    {
        pthread_create(&thread_array[i], NULL, worker_queue, NULL);
    }
    
    sequence_number = 1;                               /* Initialize seq num  */
    network_init( port );                              /* init network module */

    for ( ;; )                                         /* infinite main loop  */
    {
        network_wait();                                /* wait for clients    */
        
        for( fd = network_open(); fd >= 0; fd = network_open() )
        {
            pthread_mutex_lock( &lock_rcb );          /* lock RCB allocation  */
            
            /* Check if max num of req exceeded */
            if( req_tbl_cntr > MAX_REQ )
                pthread_cond_wait( &cond_rcb, &lock_rcb ); /* Wait if exceeded*/
            
            req_tbl_cntr++;                           /* Increment RCB counter*/
            request = malloc(sizeof(struct RCB));     /* Allocate RCB         */
            request->seq = sequence_number++;         /* Request Sequence     */
            request->fd = fd;                         /* Client ID            */
            pthread_mutex_unlock( &lock_rcb );        /* Unlock RCB allocation*/
            enqueue(queue, request);                  /* Insert in work queue */
        }
    }
    
}
