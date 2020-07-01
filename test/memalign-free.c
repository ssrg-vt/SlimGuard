#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

TEST_CASE("memalign", "[slimguard]")
{
    void *ptr = xxmemalign(128, 128);
    REQUIRE(ptr);
    REQUIRE(!((uint64_t)ptr % 128));
    memset(ptr, 0x0, 128);
    xxfree(ptr);
}
