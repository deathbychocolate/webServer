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

/* Number of supported priorities */
#define MAX_PRIORITY            3

/* head and tail of priority queues */
struct MLFQ_Q *head[MAX_PRIORITY];
struct MLFQ_Q *tail[MAX_PRIORITY];




/* admit to the end of the RR queue */
void admit_MLFQ(struct RCB *req, enum MLFP priority)
{
    struct MLFQ_Q *entry;
    entry = (struct MLFQ_Q*)malloc(sizeof(struct MLFQ_Q));
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
        tail[priority] = entry;
        entry -> next = NULL;
    }
}

struct RCB *get_MLFQ(enum MLFP priority)
{
    struct MLFQ_Q *entry = head[priority];
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
