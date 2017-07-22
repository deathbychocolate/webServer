
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

struct Queue *head_SJF;

/* -------------------------------------------------------------------------- *
 * Purpose: admit to the shortest jpb first queue according to file size
 * Parameters:
 *              req = Request control block used by scheduler
 * Returns: none
 * -------------------------------------------------------------------------- */
void admit_SJF(struct RCB *req)
{
    struct Queue *entry;
    struct Queue *ptr;
    entry = (struct Queue*)malloc(sizeof(struct Queue));
    entry -> request = req;
    /*If queue is empty or if entry has lower size than head of queue */
    if ( head_SJF == NULL || req->remainbytes < head_SJF->request->remainbytes)
    {
        entry ->next = head_SJF;
        head_SJF = entry;
    }
    /* queue has one or more elements */
    else
    {
        ptr = head_SJF;
        while ( ptr -> next != NULL &&
                    req -> remainbytes > ptr -> next -> request -> remainbytes )
        {
            ptr = ptr->next;
        }
        entry-> next = ptr -> next;
        ptr -> next = entry;
    }
}

/* -------------------------------------------------------------------------- *
 * Purpose: aquire and remove the next request from the queue
 * Parameters: none
 * Returns: Request control block used by scheduler
 * -------------------------------------------------------------------------- */
struct RCB *get_SJF ()
{
    struct Queue *entry = head_SJF;
    /* If queue is not empty*/
    if (head_SJF)
    {
        /* acquire head of the queue */
        head_SJF = head_SJF -> next;
        return entry->request;
    }
    else
        return NULL;
}
