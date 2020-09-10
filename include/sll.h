/*  sll.h
 *  SlimGuard
 *  Copyright (c) 2019, Beichen Liu, Virginia Tech
 *  All rights reserved
 */

#ifndef SLL_H
#define SLL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "debug.h"

typedef struct sll_t {
  struct sll_t *next;
} sll_t;

sll_t* add_head(sll_t* node, sll_t* slist);
sll_t* remove_head(sll_t* slist);

#endif /* SLL_H */
