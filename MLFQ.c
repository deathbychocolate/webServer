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


/* Number of supported priorities */
#define MAX_PRIORITY            3

/* head and tail of priority queues */
struct RCB *head[MAX_PRIORITY];
struct RCB *tail[MAX_PRIORITY];

/* -------------------------------------------------------------------------- *
 * Purpose: admit to the end of the MLFQ queue
 * Parameters:
 *              req: Request control block used by scheduler
 *              priority: the desired priority of the MLFQ scheduler
 * Returns:
 * -------------------------------------------------------------------------- */
void admit_MLFQ(struct RCB *req, enum MLFP priority)
{
    struct RCB *entry = req;
    /*If queue is empty */
    if ( head[priority] == NULL)
    {
        entry ->next = head[priority];
        head[priority] = entry;
    }
    /* queue has one  or more elements */
    else
    {
        tail[priority] -> next = entry;
        entry -> next = NULL;
    }
    tail[priority] = entry;
}

/* -------------------------------------------------------------------------- *
 * Purpose: aquire and remove the next request from the queue, or NULL if queue
 *                  is empty
 * Parameters:
 *              priority: the desired priority of the MLFQ scheduler
 * Returns:
 * -------------------------------------------------------------------------- */
struct RCB *get_MLFQ(enum MLFP priority)
{
    struct RCB *entry = head[priority];
    /* If queue is not empty*/
    if (head[priority])
    {
        /* acquire head of the queue */
        head[priority] = head[priority] -> next;
        return entry;
    }
    else
        return NULL;
}

/* -------------------------------------------------------------------------- *
 * Purpose: Check for the highest, not empty, priority queue.
 * Parameters: none
 * Returns: an enumerated variable indicating the top priority level available 
 * -------------------------------------------------------------------------- */
enum MLFP get_priority()
{
    if (head[HIGH_PRIORITY])
        return HIGH_PRIORITY;
    else if (head[MED_PRIORITY])
        return MED_PRIORITY;
    else
        return LOW_PRIORITY;
}
