//
//  queue.h
//  WebServer
//
//  Created by dahmi majid on 2017-07-28.
//  Copyright © 2017 Operating Systems. All rights reserved.
//

#ifndef queue_h
#define queue_h

/* Worker queue entry points */
extern void enqueue( struct work_queue *q, struct RCB *req );
extern struct RCB *dequeue( struct work_queue *q, int wait);
#endif /* queue_h */
