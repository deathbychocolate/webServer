
#ifndef SWS_H
#define SWS_H

#define MAX_HTTP_SIZE 8192                      /* size of buffer to allocate */
#define HIGH_PRIORITY_QUANTUM    MAX_HTTP_SIZE  /* High priority quantum      */
#define LOW_PRIORITY_QUANTUM     65535          /* Low  priority quantum      */

#define TRUE    1
#define FALSE   0

#include <pthread.h>

/* Request Control Block - each request is assigned an RCB */
struct RCB
{
    int seq;                              /* The sequence number of a request */
    int  fd;                              /* Client file descriptor           */
    FILE *fptr;                           /* Requested File Handle            */
    long remainbytes;                     /* Number of remaining bytes        */
    long quantum;                         /* Current quantum                  */
    char path[FILENAME_MAX];              /* path of requested file           */
    long sent;                            /* last amount of bytes sent        */
    struct RCB *next;                     /* Next RCB in the scheduler        */
};

/* Scheduler Type */
enum scheduler_type {SJF,RR,MLFQ,BAD_SCHEDULER};

/* External variables */
extern enum scheduler_type scheduler;


/* worker thread queue structure */
struct work_queue
{
    struct RCB *head;
    struct RCB *tail;
};

#endif
