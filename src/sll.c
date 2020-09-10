/*  sll.c
 *  SlimGuard
 *  Copyright (c) 2019, Beichen Liu, Virginia Tech
 *  All rights reserved
 */

#include "debug.h"
#include "sll.h"

sll_t* add_head(sll_t* node, sll_t* slist) {

  if (slist == NULL) {
    slist = node;
    slist->next = NULL;
  }  else {
    node->next = slist;
    slist = node;
  }

  return slist;
}

sll_t* remove_head(sll_t* slist) {

  if ((slist == NULL) | (slist->next == NULL))
    return NULL;

  slist = slist->next;
  return slist;
}
