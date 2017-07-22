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


struct Queue *head_RR;
struct Queue *tail_RR;


/* -------------------------------------------------------------------------- *
 * Purpose: admit to the end of the RR queue
 * Parameters:
 * Returns:
 * -------------------------------------------------------------------------- */
void admit_RR(struct RCB *req)
{
    struct Queue *entry;
    entry = (struct Queue*)malloc(sizeof(struct Queue));
    entry -> request = req;
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
 * Purpose: aquire and remove the next request from the queue
 * Parameters:
 * Returns:
 * -------------------------------------------------------------------------- */
struct RCB *get_RR ()
{
    struct Queue *entry = head_RR;
    /* If queue is not empty*/
    if (head_RR)
    {
        /* acquire head of the queue */
        head_RR = head_RR -> next;
        return entry->request;
    }
    else
        return NULL;
}
