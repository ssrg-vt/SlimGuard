#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

#define MAX_ALIGNMENT   (1024*1024)

TEST_CASE("memalign", "[slimguard]")
{
    for(int alignment = 8; alignment<=MAX_ALIGNMENT; alignment*=2) {
        void *ptr = xxmemalign(alignment, alignment);
        REQUIRE(!(ptr % alignment));
        free(ptr);

        ptr = xxmemalign(alignment, alignment*2);
        REQUIRE(!(ptr % alignment));

        ptr = xxmemalign(alignment, alignment/2);
        REQUIRE(!(ptr % alignment));
        free(ptr);
    }

}
