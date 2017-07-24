
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
};

/* Scheduler Type */
enum scheduler_type {SJF,RR,MLFQ,BAD_SCHEDULER};

/* Queue structure */
struct Queue
{
    struct RCB *request;
    struct Queue *next;
    pthread_mutex_t lock;     /* Mutex lock to prevent race condition       */
    pthread_cond_t check;     /* Communicate queue availability b/w threads */
};

/* External variables */
extern enum scheduler_type scheduler;

#endif
