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


struct RR_Q *head;
struct RR_Q *tail;

/* admit to the end of the RR queue */
void admit_RR(struct RCB *req)
{
    struct RR_Q *entry;
    entry = (struct RR_Q*)malloc(sizeof(struct RR_Q));
    entry -> request = req;
    /*If queue is empty */
    if ( head == NULL)
    {
        entry ->next = head;
        head = entry;
    }
    /* queue has one  or more elements */
    else
    {
        tail -> next = entry;
        tail = entry;
        entry -> next = NULL;
    }
}

struct RCB *get_RR ()
{
    struct RR_Q *entry = head;
    /* If queue is not empty*/
    if (head)
    {
        /* acquire head of the queue */
        head = head -> next;
        return entry->request;
    }
    else
        return NULL;
}
