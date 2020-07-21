#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

#define MAX_ALIGNMENT   (1024*1024)
#define ITERATIONS      500
#define VERBOSE         0

TEST_CASE("memalign", "[slimguard]")
{
    for(int alignment = 16; alignment<=MAX_ALIGNMENT; alignment*=2) {
        void *ptrs[ITERATIONS];
        void *ptrs2[ITERATIONS];
        void *ptrs3[ITERATIONS];

        for(int i=0; i<ITERATIONS; i++) {
            ptrs[i] = xxmemalign(alignment, alignment);
            REQUIRE(ptrs[i]);
#if VERBOSE == 1
            printf("Required alignment 0x%x got pointer @%p\n", alignment,
                    ptrs[i]);
#endif
            REQUIRE(!((uint64_t)ptrs[i] % alignment));
            memset(ptrs[i], 0x0, alignment);
        }

        for(int i=0; i<ITERATIONS; i++)
            xxfree(ptrs[i]);

        for(int i=0; i<ITERATIONS; i++) {
            ptrs2[i] = xxmemalign(alignment, alignment*2);
#if VERBOSE == 1
            printf("Required alignment 0x%x got pointer @%p\n", alignment,
                    ptrs2[i]);
#endif
            REQUIRE(ptrs2[i]);
            REQUIRE(!((uint64_t)ptrs2[i] % alignment));
            memset(ptrs2[i], 0x0, alignment*2);
        }

        for(int i=0; i<ITERATIONS; i++)
            xxfree(ptrs2[i]);

        for(int i=0; i<ITERATIONS; i++) {
            ptrs3[i] = xxmemalign(alignment, alignment/2);
#if VERBOSE == 1
            printf("Required alignment 0x%x got pointer @%p\n", alignment,
                    ptrs3[i]);
#endif
            REQUIRE(ptrs3[i]);
            REQUIRE(!((uint64_t)ptrs3[i] % alignment));
            memset(ptrs3[i], 0x0, alignment/2);
        }

        for(int i=0; i<ITERATIONS; i++)
            xxfree(ptrs3[i]);

    }

}
