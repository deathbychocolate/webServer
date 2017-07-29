
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sws.h"
#include "scheduler.h"


/* Mutual exclusion lock */
pthread_mutex_t lock_schedule = PTHREAD_MUTEX_INITIALIZER;

/* -------------------------------------------------------------------------- *
 * Purpose: Admit a request to the user specified scheduler queue
 * Parameters: 
 *                  req: client request to be admitted
 * Returns: none
 * -------------------------------------------------------------------------- */
void admit_scheduler(struct RCB *req)
{
    switch (scheduler)
    {
        case RR:
            admit_RR(req);
            break;
        case SJF:
            admit_SJF(req);
            break;
        case MLFQ:
            admit_MLFQ(req,HIGH_PRIORITY);
            break;
    }
}

/* -------------------------------------------------------------------------- *
 * Purpose: Check the scheduler type specified by user
 * Parameters: 
 *                  scheduler: the name of the scheduler
 * Returns: an enumerated interger for the scheduler type and an indication if
 *                  if the scheduler is not supported.
 * -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- *
 * Purpose: Acquire the next request specified by the scheduler
 * Parameters: none
 * Returns: The request control block that is removed from the scheduler
 * -------------------------------------------------------------------------- */
struct RCB *get_next_scheduler()
{
    switch (scheduler)
    {
        case RR:
            return get_RR();
            break;
        case SJF:
            return get_SJF();
            break;
        case MLFQ:
            return get_MLFQ(get_priority());
            break;
        case BAD_SCHEDULER:
            break;
    }
    return NULL;
}

/* -------------------------------------------------------------------------- *
 * Purpose: resubmit a request to the scheduler. Only used for RR and MLFQ
 * Parameters:
 *                  req: client request to be admitted
 * Returns: none
 * -------------------------------------------------------------------------- */
void resubmit_scheduler(struct RCB *req)
{
    switch (scheduler)
    {
        case RR:
            admit_RR(req);
            break;
        case MLFQ:
            /* Check if request was in high priority queue */
            if (req->quantum == HIGH_PRIORITY_QUANTUM)
            {
                req->quantum = LOW_PRIORITY_QUANTUM;
                admit_MLFQ(req,MED_PRIORITY); /* Submit to med-priority queue */
            }
            else /* request was in med priority queue */
            {
                req->quantum = req->remainbytes;
                admit_MLFQ(req,LOW_PRIORITY); /* Submit to low-priority queue */
            }
            break;
    } /* SJF not handled, no need for resubmission*/
}

/* -------------------------------------------------------------------------- *
 * Purpose: submit an RCB and ensure scheduler protection by using mutex locks
 * Parameters: 
 *                  new: The RCB to be admitted to the scheduler
 *                  admit: variable to identfiy whether to admit or resubmit RCB
 * Returns: none
 * -------------------------------------------------------------------------- */
void safe_enqueue(struct RCB *new, int admit)
{
    pthread_mutex_lock(&lock_schedule);                 /* Lock scheduler */
    if (admit == 1)                                     /* Admit RCB*/
        admit_scheduler(new);
    else                                                /* Else resubmit RCB */
        resubmit_scheduler(new);
    pthread_mutex_unlock(&lock_schedule);               /* Unlock scheduler */
}

/* -------------------------------------------------------------------------- *
 * Purpose: acquire an RCB and ensure scheduler protection by using mutex locks
 * Parameters: none
 * Returns: the RCB acquired from the scheduler
 * -------------------------------------------------------------------------- */
struct RCB *safe_dequeue()
{
    pthread_mutex_lock(&lock_schedule);                 /* Lock scheduler */
    struct RCB *new = get_next_scheduler();             /* Get request */
    pthread_mutex_unlock(&lock_schedule);               /* Unlock scheduler */
    return new;
}
