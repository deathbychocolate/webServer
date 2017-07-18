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
#include "heap.h"
#include "sws.h"

#define MAX_HTTP_SIZE 8192                      /* size of buffer to allocate */
#define DEFAULT_HEAP_SIZE 1024

/* This function takes a file handle to a client, reads in the request,
 *    parses the request, and sends back the requested file.  If the
 *    request is improper or the file is not available, the appropriate
 *    error is sent back.
 * Parameters:
 *             fd : the file descriptor to the client connection
 * Returns: None
 */

pthread_mutex_t mutex;
int sequence_number;
enum scheduler_type s_type;

static void serve_client( int fd )
{
    //pthread_mutex_lock(&mutex);
    static char *buffer;                              /* request buffer       */
    char *req = NULL;                                 /* ptr to req file      */
    char *brk;                                        /* state used by strtok */
    char *tmp;                                        /* error checking ptr   */
    FILE *fin;                                        /* input file handle    */
    int len;                                          /* length of data read  */
    long int fileSize;

    if ( !buffer )                                    /* 1st time, alloc buffer */
    {
        buffer = malloc( MAX_HTTP_SIZE );
        if ( !buffer )                                  /* error check */
        {
            perror( "Error while allocating memory" );
            abort();
        }
    }

    memset( buffer, 0, MAX_HTTP_SIZE );
    if ( read( fd, buffer, MAX_HTTP_SIZE ) <= 0 )     /* read req from client */
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
        write( fd, buffer, len );                       /* if not, send err */
    }
    else                                              /* if so, open file */
    {
        req++;                                          /* skip leading / */
        fin = fopen( req, "r" );
        fseek(fin, 0, SEEK_END);
        fileSize = ftell(fin);
        fseek(fin, 0, SEEK_SET);
        printf("The file size is: %ld bytes", fileSize);
                                /* open file */
        if ( !fin )                                    /* check if successful */
        {
            len = sprintf( buffer, "HTTP/1.1 404 File not found\n\n" );
            write( fd, buffer, len );                  /* if not, send err */
        }
        else                                               /* if so, send file */
        {
            len = sprintf( buffer, "HTTP/1.1 200 OK\n\n" );/* send success code */
            write( fd, buffer, len );

            do                                            /* loop, read & send file */
            {
                len = fread( buffer, 1, MAX_HTTP_SIZE, fin );  /* read file chunk */
                if ( len < 0 )                              /* check for errors */
                {
                    perror( "Error while writing to client" );
                }
                else if ( len > 0 )                         /* if none, send chunk */
                {
                    len = write( fd, buffer, len );
                    if ( len < 1 )                            /* check for errors */
                    {
                        perror( "Error while writing to client" );
                    }
                }
            }
            while ( len == MAX_HTTP_SIZE );               /* the last chunk < 8192 */
            fclose( fin );
        }
    }
    close( fd );                                     /* close client connection*/
}

void admit_request()
{
    
}

/* Check the scheduler type specified by user */
enum scheduler_type scheduler_init(char *scheduler)
{
    if (!strcasecmp(scheduler, "SJF"))
        return SJF;
    
    else if (!strcasecmp(scheduler, "RR"))
        return RR;
    
    else if (!strcasecmp(scheduler, "MLFQ"))
        return MLFQ;
    
    else
        return BAD_SCHEDULER;
}

void admit_scheduler(struct RCB req)
{
    switch (s_type)
    {
        case RR:
            break;
        case SJF:
            break;
        case MLFQ:
            break;
        case BAD_SCHEDULER:
        default:
            break;
    }
}

/* This function is where the program starts running.
 *    The function first parses its command line parameters to determine port #
 *    Then, it initializes, the network and enters the main loop.
 *    The main loop waits for a client (1 or more to connect, and then processes
 *    all clients by calling the seve_client() function for each one.
 * Parameters:
 *             argc : number of command line parameters (including program name
 *             argv : array of pointers to command line parameters
 * Returns: an integer status code, 0 for success, something else for error.
 */
int main( int argc, char **argv )
{
    int port = -1;                                  /* server port #          */
    int fd;                                         /* client file descriptor */

    /* Check for and process parameters */
    if ( argc < 3 )
    {
        printf( "usage: sws <port> <scheduler>\n" );
        return 0;
    }
    
    /* Get port number */
    sscanf( argv[1], "%d", &port );
    
    /* Check if port is within specified range */
    if(port < 1024 || port > 65335)
    {
        printf( "Inaccessible port\n" );
        return 0;
    }
    
    /* Check if Scheduler is supported */
    if ((s_type = scheduler_init(argv[2])) == BAD_SCHEDULER)
    {
        printf( "Unsupported Scheduler\n" );
        return 0;
    }
    sequence_number = 1;
    network_init( port );                             /* init network module */

    for ( ;; )                                        /* main loop */
    {
        network_wait();                                 /* wait for clients */

        /* Admit to scheduler */
        for ( fd = network_open(); fd >= 0; fd = network_open() )  /* get clients */
        {
            serve_client( fd );                           /* process each client */
        }
        
        /* Serve request */
        
    }
}
