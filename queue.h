//queue.h

#ifndef QUEUE_H
#define QUEUE_H

typedef struct reqInfo
{
    long int fileSize;
} reqInfo;

typedef struct node
{
    struct node *next;
    reqInfo *ri;
} node;

static node *head;

int addJob(reqInfo ri);
reqInfo* getJob();


#endif