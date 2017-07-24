/*
 * Author: Aj
 * Course:
 * Date created:
 * File name :
 * Purpose:
 * acknowledgement:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sws.h"
#include "scheduler.h"

int quantum_init[] = {MAX_HTTP_SIZE,MAX_HTTP_SIZE,0};


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
        case BAD_SCHEDULER:
        default:
            break;
    }
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
    }
    return NULL;
}

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
