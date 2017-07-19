//
//  scheduler.c
//  WebServer
//
//  Created by dahmi majid on 2017-07-19.
//  Copyright © 2017 Operating Systems. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sws.h"
void admit_scheduler(struct RCB *req)
{
    switch (s_type)
    {
        case RR:
            //admit_RR(req);
            break;
        case SJF:
            //admit_SJF(req);
            break;
        case MLFQ:
            //admit_MLFQ(req,);
            break;
        case BAD_SCHEDULER:
        default:
            break;
    }
}
