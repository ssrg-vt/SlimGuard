#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>
#include <assert.h>

#define MAX_ALIGNMENT   (1024*1024)

int main(int argc, char **argv) {
    for(int alignment = 16; alignment<=MAX_ALIGNMENT; alignment*=2) {

        void *ptr = memalign(alignment, alignment);
        assert(ptr);
        printf("Required size 0x%x with alignment 0x%x got pointer @%p\n",
                alignment, alignment, ptr);
        assert(!((uint64_t)ptr % alignment));
        memset(ptr, 0x0, alignment);
        free(ptr);

        void *ptr2 = memalign(alignment, alignment*2);
        printf("Required size 0x%x with alignment 0x%x got pointer @%p\n",
                alignment*2, alignment, ptr2);
        assert(ptr2);
        assert(!((uint64_t)ptr2 % alignment));
        memset(ptr2, 0x0, alignment*2);
        free(ptr2);

        void *ptr3 = memalign(alignment, alignment/2);
        printf("Required size 0x%x with alignment 0x%x got pointer @%p\n",
                alignment/2, alignment, ptr3);
        assert(ptr3);
        assert(!((uint64_t)ptr3 % alignment));
        memset(ptr3, 0x0, alignment/2);
        free(ptr3);

    }
}
