/*
 *  slimguard-large.h
 *  SlimGuard
 *  Copyright (c) 2019, Beichen Liu, Virginia Tech
 *  All rights reserved
 */

#ifndef SG_LARGE_H
#define SG_LARGE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include "debug.h"


typedef struct large_obj_t {
  void* start_addr;
  uint32_t align_size;
  struct large_obj_t *next;
} large_obj_t;

large_obj_t *next_obj(large_obj_t *node);
void add_large(void *addr, uint32_t align_sz);
void remove_large( void *target);
large_obj_t *in_list(void *target);

void print_large();

void *xxmalloc_large(size_t sz);
int xxfree_large(void *ptr);

#endif /* SG_LARGE_H */ 
