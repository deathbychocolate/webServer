
#include <stdio.h>
#include <stdlib.h>
#include "sws.h"
#include "scheduler.h"

struct RCB *head_SJF;

/* -------------------------------------------------------------------------- *
 * Purpose: admit to the shortest job first queue according to file size
 * Parameters:
 *              req: Request control block used by scheduler
 * Returns: none
 * -------------------------------------------------------------------------- */
void admit_SJF(struct RCB *req)
{
    struct RCB *entry = req;
    struct RCB *ptr;
    /*If queue is empty or if entry has lower size than head of queue */
    if ( head_SJF == NULL || req->remainbytes < head_SJF->remainbytes)
    {
        entry ->next = head_SJF;
        head_SJF = entry;
    }
    /* queue has one or more elements */
    else
    {
        ptr = head_SJF;
        while ( ptr -> next != NULL &&
                    req -> remainbytes > ptr -> next -> remainbytes )
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
    struct RCB *entry = head_SJF;
    /* If queue is not empty*/
    if (head_SJF)
    {
        /* acquire head of the queue */
        head_SJF = head_SJF -> next;
        return entry;
    }
    else
        return NULL;
}
