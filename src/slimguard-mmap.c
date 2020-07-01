#include "slimguard-mmap.h"
#include <err.h>
#include <sys/mman.h>
#include <stdlib.h>

void * slimguard_mmap(uint64_t size) {
   void *r = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON,
           -1, 0);

   if(r == MAP_FAILED)
       err(-1, "!mmap(%lu)", size);

   return r;
}

void *slimguard_aligned_mmap(uint64_t align, uint64_t size) {

    if(align % 0x1000)
        errx(-1, "Bad alignment 0x%lx for slimguard_aligned_mmap\n", align);

    void *r = mmap(0, size + align, PROT_READ | PROT_WRITE, MAP_PRIVATE |
           MAP_ANON, -1, 0);

    if(r == MAP_FAILED)
        err(-1, "!mmap(%lu)", size);

    void *addr = (void *)(((uint64_t)r + align - 1) & ~(align - 1));
    munmap(r, (uint64_t)addr - (uint64_t)r);

    return addr;
}
