/*
 * ***************************************************
 *  My Implementation of a Hash Map/Hash Table in C
 *
 *  Code by Treyse Miller, September 2023
 * ***************************************************
 */

#ifndef TM_HASHMAP_H
#define TM_HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HT_MAX_KEY      256
#define HT_MAX_SIZE     10


// Returns the index of the key
// For ht_size, use the HT_GET_SIZE macro
unsigned int HT_HASH_FUNC(const char *key, size_t ht_size){
    int len = strnlen(key, HT_MAX_KEY);
    unsigned int index = 0;
    for(int i = 0; i < len; i++){
        index += key[i];
        index = (index * key[i]) % ht_size;
    }
    return index;
}

// The key type is always a char *
#define HT_ITEM(val_type) \
    struct{ \
        char *key; \
        val_type val; \
    }


// When creating a hash table, use a pointer to it
#define HT_TABLE(item_type) \
    struct{ \
        item_type **items; \
        size_t size; \
        size_t item_size; \
        int count; \
    }


#define HT_GET_SIZE(table_ptr) \
    table_ptr->size \


#define HT_INIT(table_ptr, table_type, item_type, init_size)       \
    (table_type *)malloc(sizeof(table_type)); \
    table_ptr->size = init_size; \
    table_ptr->count = 0;  \
    table_ptr->item_size = sizeof(item_type);  \
    table_ptr->items = calloc(table_ptr->size, sizeof(void *)); \
    for(int i = 0; i < table_ptr->size; i++){ \
        table_ptr->items[i] = NULL; \
    }


#define HT_CREATE_ITEM(item_ptr, item_type, key_, value) \
{ \
    item_ptr = (item_type *)malloc(sizeof(item_type)); \
    item_ptr->key = (char *)malloc(strlen(key_) + 1); \
    strcpy(item_ptr->key, key_); \
    item_ptr->val = value; \
}


#define HT_INSERT(table_ptr, item_ptr) \
{ \
    if(item_ptr != NULL){ \
        int index = HT_HASH_FUNC(item_ptr->key, HT_GET_SIZE(table_ptr)); \
        if(table_ptr->items[index] != NULL){ \
            printf("Hash collision\n"); \
            HT_FREE(table_ptr); \
            exit(EXIT_FAILURE); \
        } \
        table_ptr->items[index] = item_ptr; \
    } \
}


#define HT_AT(table_ptr, key_, item_ptr) \
{ \
    int index = HT_HASH_FUNC(key_, HT_GET_SIZE(table_ptr)); \
    if(table_ptr->items[index] != NULL \
        && strncmp(table_ptr->items[index]->key, key_, HT_GET_SIZE(table_ptr)) == 0){ \
            item_ptr = table_ptr->items[index]; \
    } \
    else{ \
        item_ptr = NULL; \
    } \
}


#define HT_FREE_ITEM(item_ptr) \
{ \
    free(item_ptr->key); \
    free(item_ptr); \
}

#define HT_FREE(table_ptr) \
{ \
    for(int i = 0; i < table_ptr->size; i++){ \
        if(table_ptr->items[i] != NULL){ \
            HT_FREE_ITEM(table_ptr->items[i]); \
        } \
    } \
    free(table_ptr->items); \
    free(table_ptr); \
}

#define HT_PRINT(table_ptr) \
    printf("\n"); \
    for(int i = 0; i < table_ptr->size; i++){ \
        if(table_ptr->items[i] == NULL){ \
            printf("\t%i\t---\n", i); \
        } \
        else{ \
            printf("\t%i\t%s\t%i\n", i, table_ptr->items[i]->key, table_ptr->items[i]->val); \
        } \
    }


// Typedef naming for HT_ITEM is HT_ITEM_VALTYPE (HT is for hash table)
typedef HT_ITEM(int) HT_ITEM_INT;

// Typedef naming for HT_TABLE is HT_VALTYPE (HT is for hash table)
// Should be a pointer
typedef HT_TABLE(HT_ITEM_INT) HT_INT;


#endif  // TM_HASHMAP_H