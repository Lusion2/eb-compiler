#ifndef TM_DYNLIST_H
#define TM_DYNLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#define dynlist(type) \
    struct{ \
        type *data; \
        size_t size; \
        size_t element_size; \
        int index; \
    } 

#define dynlist_init(list, type) \
    list.element_size = sizeof(type); \
    list.size = 0; \
    list.index = 0; \
    list.data = (type *)malloc(list.element_size)


#define dynlist_push(list, new_element) \
    list.size++; \
    list.data = realloc(list.data, list.size * list.element_size); \
    list.data[list.size - 1] = new_element 


#define dynlist_push_ptr(list_ptr, new_element) \
    list_ptr->size++; \
    list_ptr->data = realloc(list_ptr->data, list_ptr->size * list_ptr->element_size); \
    list_ptr->data[list_ptr->size - 1] = new_element 


#define dynlist_free(list) \
{ \
    free(list.data); \
    list.size = 0; \
}


#define dynlist_free_ptr(list_ptr) \
{\
    free(list_ptr->data); \
    list_ptr->size = 0; \
}

#define dynlist_increment(list) \
    if(!(list.index + 1 >= list.size)){ \
        list.index++; \
    } \
    else{ \
        fprintf(stderr, "ERROR: Dynlist index out of range\n"); \
        exit(EXIT_FAILURE); \
    }


typedef dynlist(int) dynlist_int;
typedef dynlist(long) dynlist_long;
typedef dynlist(float) dynlist_float;
typedef dynlist(double) dynlist_double;
typedef dynlist(char) dynlist_char;

#endif