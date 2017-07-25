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
struct Queue *head[MAX_PRIORITY];
struct Queue *tail[MAX_PRIORITY];

/* -------------------------------------------------------------------------- *
 * Purpose: admit to the end of the MLFQ queue
 * Parameters:
 *                  req: the 
 * Returns:
 * -------------------------------------------------------------------------- */
void admit_MLFQ(struct RCB *req, enum MLFP priority)
{
    struct Queue *entry;
    entry = (struct Queue*)malloc(sizeof(struct Queue));
    entry -> request = req;
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
        //tail[priority] = entry;
        entry -> next = NULL;
    }
    tail[priority] = entry;
}

/* -------------------------------------------------------------------------- *
 * Purpose: aquire and remove the next request from the queue
 * Parameters:
 * Returns:
 * -------------------------------------------------------------------------- */
struct RCB *get_MLFQ(enum MLFP priority)
{
    struct Queue *entry = head[priority];
    /* If queue is not empty*/
    if (head[priority])
    {
        /* acquire head of the queue */
        head[priority] = head[priority] -> next;
        return entry->request;
    }
    else
        return NULL;
}

/* -------------------------------------------------------------------------- *
 * Purpose: Check for the highest, not empty, priority queue.
 * Parameters:
 * Returns:
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
