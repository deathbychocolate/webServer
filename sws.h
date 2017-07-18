
#ifndef SWS_H
#define SWS_H

/* Request Control Block - each request is assigned an RCB */
struct RCB
{
    int seq;                              /* The sequence number of a request */
    int  fd;                              /* Client file descriptor           */
    FILE *fptr;                           /* Requested File Handle            */
    int remainbytes;                      /* Number of remaining bytes        */
    int quantum;                          /* Current quantum                  */
};


/* Scheduler Type */
enum scheduler_type {BAD_SCHEDULER,SJF,RR,MLFQ};

/* RR queue */
struct RR_Q
{
    struct RCB request;
    struct RR_Q *next;
};

/* MLFQ queue */
struct MLFQ_Q
{
    struct RCB request;
    struct MLFQ_Q *next;
};

#endif
