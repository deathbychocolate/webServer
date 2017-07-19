
#ifndef SWS_H
#define SWS_H

#define MAX_HTTP_SIZE 8192                      /* size of buffer to allocate */


/* Request Control Block - each request is assigned an RCB */
struct RCB
{
    int seq;                              /* The sequence number of a request */
    int  fd;                              /* Client file descriptor           */
    FILE *fptr;                           /* Requested File Handle            */
    long remainbytes;                     /* Number of remaining bytes        */
    int quantum;                          /* Current quantum                  */
};


/* Scheduler Type */
enum scheduler_type {BAD_SCHEDULER,SJF,RR,MLFQ};

/* Multi Level Feedback Priority */
enum MLFP {HIGH_PRIORITY, MED_PRIORITY, LOW_PRIORITY};

/* RR queue */
struct RR_Q
{
    struct RCB *request;
    struct RR_Q *next;
};

/* MLFQ queue */
struct MLFQ_Q
{
    struct RCB *request;
    struct MLFQ_Q *next;
};

/* External variables */
extern enum scheduler_type s_type;

#endif
