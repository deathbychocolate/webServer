
#include <stdio.h>
#include "sws.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond  = PTHREAD_COND_INITIALIZER;

/* -------------------------------------------------------------------------- *
 * Purpose: Inserting an RCB into the work queue used by worker threads
 * Parameters: 
 *                  q: worker queue to have RCB inserted into
 *                  req: Request control block admitted into queue
 * Returns: none
 * -------------------------------------------------------------------------- */
void enqueue( struct work_queue *q, struct RCB *req )
{
    pthread_mutex_lock( &lock );                    /* Lock queue */
    req->next = NULL;
    if( !q->head ) {                                /* if queue is empty */
        q->head = req;
    } else {                                        /* else queue not empty */
        q->tail->next = req;
    }
    q->tail = req;
    pthread_cond_signal( &cond );               /* Signal buffer is not empty*/
    pthread_mutex_unlock( &lock );              /* Unlock queue */
}

/* -------------------------------------------------------------------------- *
 * Purpose: remove an RCB from the work queue by a worker thread
 * Parameters: 
 *                  wait: ensure threads wait initially
 *                  q: worker queue to have RCB inserted into
 * Returns: the RCB acquired from the work queue
 * -------------------------------------------------------------------------- */
struct RCB *dequeue( struct work_queue *q, int wait)
{
    struct RCB *req;
    pthread_mutex_lock( &lock );                  /* Lock queue */
    if( wait && q->head == NULL)                  /* If empty */
    {
        pthread_cond_wait( &cond, &lock );        /* Wait and unlock */
    }
    
    req = q->head;                                /* Acquire head of the queue*/
    if( q->head ) {                               /* if not empty */
        q->head = q->head->next;                  /* remove first element */
        req->next = NULL;
    }
    pthread_mutex_unlock( &lock );                /* unlock queue */
    return req;
}
