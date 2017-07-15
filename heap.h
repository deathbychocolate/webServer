//heap.h

#ifndef HEAP_H
#define HEAP_H

typedef struct fileInfo
{
    long int fileSize;
    long int remaining;
} fileInfo;

typedef struct heap
{
    struct fileInfo *heap;
    void *heapStart;
    void *heapEnd;
    void *currentStart;
    void *currentEnd;
}

void Insert(fileInfo);
int deleteMin();
int reheap(heap, int mode);

#endif