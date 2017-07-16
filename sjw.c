//sjf.c

#include "queue.h"
#include <stdlib.h>

//returns an int if it succeeded or not.
int addJob(reqInfo *ri)
{
    //initialize the list
    if (!head)
    {
        head = malloc(sizeof(node));
        head->ri = ri;
        head->next = NULL;
        return 1;
    }
    node *current;
    else if (!head->next)
    {
        head->next = malloc(sizeof(node));
        current = head->next;
        current->ri = ri;
        current->next = NULL;
        return 1;
    }

    node *prev;
    prev = head;
    current = head->next;
    if (head->ri->fileSize > ri->fileSize)
    {
        head = malloc(sizeof(node));
        head->next = prev;
        head->ri = ri;
        return 1;
    }
    //determine where to insert the job
    while (current)
    {
        if (current->ri->fileSize > ri->fileSize)
        {
            prev->next = malloc(sizeof(node));
            prev->next->next = current;
            prev->next->ri = ri;
            return 1;
        }
        prev = current;
        current = current->next;
    }
    prev->next = malloc(sizeof(node));
    prev->next->ri = ri;
    prev->next->next = NULL;
    return 1;
}

reqInfo* getJob()
{
    if (!head) return NULL;
    reqInfo *ri = head->ri;
    node prev = head;
    head = head->next;
    free(head);
    return ri;
}