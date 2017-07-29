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
#include "sws.h"
#include "scheduler.h"


struct RCB *head_RR;
struct RCB *tail_RR;


/* -------------------------------------------------------------------------- *
 * Purpose: admit to the end of the RR queue
 * Parameters:
 *              req: Request control block used by scheduler
 * Returns:
 * -------------------------------------------------------------------------- */
void admit_RR(struct RCB *req)
{
    struct RCB *entry = req;
    /*If queue is empty */
    if ( head_RR == NULL)
    {
        entry ->next = head_RR;
        head_RR = entry;
    }
    /* queue has one or more elements */
    else
    {
        tail_RR -> next = entry;
        entry -> next = NULL;
    }
    tail_RR = entry;
}

/* -------------------------------------------------------------------------- *
 * Purpose: aquire and remove the next request from the queue or NULL if not 
 *              scheduler is empty
 * Parameters:
 * Returns: Request control block acquired from scheduler
 * -------------------------------------------------------------------------- */
struct RCB *get_RR ()
{
    struct RCB *entry = head_RR;
    /* If queue is not empty*/
    if (head_RR)
    {
        /* acquire head of the queue */
        head_RR = head_RR -> next;
        return entry;
    }
    else
        return NULL;
}
