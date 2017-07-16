//heap.h

#ifndef HEAP_H
#define HEAP_H

typedef struct fileInfo
{
    long int fileSize;
    long int remaining;
    int numRuns; //this will be needed for the mfq
} fileInfo;

// heap structure
typedef struct heap
{
    struct fileInfo *heap;
    void *heapStart;
    void *heapEnd;
    void *currentStart;
    void *currentEnd;
}
//use this to add something to the queue, this will automatically reheap
void Insert(fileInfo);
//removes the first item, so use this after a chunk has been written
int deleteMin();
//the algorithm that reorders our heap/array
int reheap(heap, int mode);

#endif
