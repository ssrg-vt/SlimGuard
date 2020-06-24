#include "slimguard-mmap.h"
#include <err.h>
#include <sys/mman.h>
#include <stdlib.h>

void * slimguard_mmap(uint64_t size) {
   void *r = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON,
           -1, 0);

   if(r == MAP_FAILED) {
       r = NULL;
       errx(-1, "!mmap(%llu)", size);
   }

   return r;
}

