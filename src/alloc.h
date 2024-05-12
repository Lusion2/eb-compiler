#ifndef TM_ARENA_ALLOC_H
#define TM_ARENA_ALLOC_H

#include <stdlib.h>

typedef unsigned char byte;

/*
 * Creates a heap of variable size
 * The heap is in bytes
 */
#define HEAP_CREATE \
    struct { \
        byte *data; \
        byte *offset; \
        size_t size; \
    } 

/*
 * Initializes the heap with a set size
 */
#define HEAP_INIT(heap, heap_size) \
    heap.data = (byte *)malloc(heap_size * sizeof(byte)); \
    heap.offset = heap.data; \
    heap.size = heap_size;

/*
 * Returns a pointer to 'var' after being allocated
 */
#define HEAP_ALLOC(heap, type, var) \
    { \
    void *offset = heap.offset; \
    heap.offset += sizeof(type); \
    var = (type *)offset; \
    }


#define HEAP_FREE(heap) \
    free(heap.data)

typedef HEAP_CREATE HEAP;

#endif