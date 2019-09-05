/*
 *  slimguard-large.c
 *  SlimGuard
 *  Copyright (c) 2019, Beichen Liu, Virginia Tech
 *  All rights reserved
 */

#include "debug.h"
#include "slimguard-large.h"

#define N (1<<10) // number of large object

pthread_mutex_t lock;
large_obj_t* large_list = NULL;

large_obj_t*
next_obj(large_obj_t *node) 
{
    return node->next;
};

void 
add_large(void *addr, 
          uint32_t align_sz)
{
    if (large_list == NULL) {
        large_list = (large_obj_t *)mmap(NULL, 
                                         sizeof(struct large_obj_t *), 
                                         PROT_READ|PROT_WRITE, 
                                         MAP_PRIVATE|MAP_ANON, 
                                         -1, 
                                         0);
        if (large_list == NULL) {
            Error("fails to mmap for size %lu\n", sizeof(struct large_list *));
            exit(-1);
        }

        large_list->start_addr = addr;
        large_list->align_size = align_sz;
        large_list->next = NULL;

        return;
    }

    large_obj_t *node = (large_obj_t *)mmap(NULL, 
                                            sizeof(struct large_obj_t *), 
                                            PROT_READ|PROT_WRITE, 
                                            MAP_PRIVATE|MAP_ANON, 
                                            -1, 
                                            0);

    if (node == NULL) {
        Error("fails to mmap for size %lu\n", sizeof(struct large_obj_t *));
        exit(-1);
    }

    node->start_addr = addr;
    node->align_size = align_sz;
    node->next = large_list;

    large_list = node;
}

void 
remove_large(void *target)
{
    large_obj_t *tmp = large_list, *pre;

    if ((tmp->next == NULL) | (tmp == NULL)) {
        large_list = NULL;
    }

    if ((tmp != NULL) && (tmp->start_addr == target)) {
        large_list = tmp->next;
        return;
    }
  
    while ((tmp != NULL) && (tmp->start_addr !=target)) { 
        pre = tmp; 
        tmp = tmp->next; 
    } 

    if (tmp == NULL) { 
        return; 
    }  

    pre->next = tmp->next; 
}

large_obj_t* 
in_list(void *target)
{
    if (large_list == NULL) {
        return NULL;
    }

    large_obj_t *curr = large_list;

    while (curr != NULL) {
        if (curr->start_addr == target) {
            return curr;
        }

        curr = next_obj(curr);
    }

    return NULL;
}

void 
print_large()
{
    large_obj_t *tmp = large_list;

    while (tmp) {
        fprintf(stderr, "start %p\n", tmp->start_addr);
        tmp = tmp->next;
    }
}

void*
xxmalloc_large(size_t sz)
{
    uint32_t need;
    if (sz & 0xff) {
        need = ((sz >> 8)+1) << 8;
    }
    else {
        need = sz;
    }

    void *ret = mmap(NULL, 
                     need, 
                     PROT_READ|PROT_WRITE, 
                     MAP_PRIVATE|MAP_ANON, 
                     -1, 
                     0);
    
    if (ret == NULL) {
        Error("fails to mmap for size %lu\n", sz);
        exit(-1);
    }

    /* Lock here */
    pthread_mutex_lock(&lock);
    add_large(ret, need);
    pthread_mutex_unlock(&lock);
    /* Lock end */

    return ret;
}

int 
xxfree_large(void *ptr)
{
    large_obj_t *ret;
    ret = in_list(ptr);

    if(ret) {
        munmap(ptr, ret->align_size);

        /* Lock here */
        pthread_mutex_lock(&lock);
        remove_large(ptr);
        pthread_mutex_unlock(&lock);
        /* Lock end */

        return 1;
    }
    else {
        Error("Invalid address: %p\n", ptr);

        return -1;
    }

    return 0;
}
