#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

#define ALLOC_SIZE  (5*1024*1024)

TEST_CASE("large-alloc", "[slimguard]") {
    void *ptr = xxmalloc(ALLOC_SIZE);
    REQUIRE(ptr);
    memset(ptr, 0x0, ALLOC_SIZE);
    xxfree(ptr);
}
