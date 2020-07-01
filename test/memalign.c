#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

#define MAX_ALIGNMENT   (1024*1024)

TEST_CASE("memalign", "[slimguard]")
{
    for(int alignment = 16; alignment<=MAX_ALIGNMENT; alignment*=2) {
        void *ptr = xxmemalign(alignment, alignment);
        REQUIRE(ptr);
        printf("Required alignment 0x%x got pointer @%p\n", alignment, ptr);
        REQUIRE(!((uint64_t)ptr % alignment));
        memset(ptr, 0x0, alignment);
        xxfree(ptr);

        ptr = xxmemalign(alignment, alignment*2);
        REQUIRE(ptr);
        REQUIRE(!((uint64_t)ptr % alignment));
        memset(ptr, 0x0, alignment*2);
        xxfree(ptr);

        ptr = xxmemalign(alignment, alignment/2);
        REQUIRE(ptr);
        REQUIRE(!((uint64_t)ptr % alignment));
        memset(ptr, 0x0, alignment/2);
        xxfree(ptr);
    }

}
