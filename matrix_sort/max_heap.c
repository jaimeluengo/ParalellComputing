#include <stdlib.h>
#include <limits.h>
#include "max_heap.h"



/*Initialize Heap*/
max_heap_t* HeapInit(int size)
{
    max_heap_t* heap_struct = malloc(sizeof(max_heap_t));
    heap_struct->heap = malloc(size*sizeof(int));
    return heap_struct;
}

/*Insert an element into the heap */
void insert_heap(max_heap_t* heap_struct, int element)
{
    heap_struct->heapSize++;
    heap_struct->heap[heapSize] = element; /*Insert in the last place*/
    /*Adjust its position*/
    int now = heap_struct->heapSize;
    while (heap_struct->heap[now / 2] < element)
    {
        heap_struct->heap[now] = heap_struct->heap[now / 2];
        now /= 2;
    }
    heap_struct->heap[now] = element;
}

int delete_max_heap(max_heap_t* heap_struct)
{

    /* heap[1] is the minimum element. So we remove heap[1]. Size of the heap is decreased.
         Now heap[1] has to be filled. We put the last element in its place and see if it fits.
         If it does not fit, take minimum element among both its children and replaces parent with it.
         Again See if the last element fits in that place.*/
    int maxElement, lastElement, child, now;
    maxElement = heap_struct->heap[1];
    lastElement = heap_struct->heap[heap_struct->heapSize--];
    /* now refers to the index at which we are now */
    for (now = 1; now * 2 <= heap_struct->heapSize; now = child)
    {
        /* child is the index of the element which is minimum among both the children */
        /* Indexes of children are i*2 and i*2 + 1*/
        child = now * 2;
        /*child!=heapSize beacuse heap[heapSize+1] does not exist, which means it has only one
             child */
        if (child != heap_struct->heapSize && heap_struct->heap[child + 1] > heap_struct->heap[child])
        {
            child++;
        }
        /* To check if the last element fits ot not it suffices to check if the last element
             is less than the minimum element among both the children*/
        if (lastElement < heap_struct->heap[child])
        {
            heap_struct->heap[now] = heap_struct->heap[child];
        }
        else /* It fits there */
        {
            break;
        }
    }
    heap_struct->heap[now] = lastElement;
    return maxElement;
}