#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

/*
TEST_CASE("invalid/double free test", "[slimguard]")
{
    void *ptr = xxmalloc(10);
    xxfree(ptr);
    xxfree(ptr);
    REQUIRE_THROWS(exit(6));
}

TEST_CASE("overflow test", "[slimguard]")
{
    void* ptr = xxmalloc(19);
    memset(ptr, 0, 100);
    xxfree(ptr);
    //REQUIRE_ABORT(xxfree(ptr));
}
*/
TEST_CASE("bitmap", "[slimguard]")
{
    int num = 1000000;
    void *ptr[num];

    for (int i = 0; i < num; i++) {
        ptr[i] = xxmalloc(128);
    }

    for (int i = (num-1); i >= 0; i--) {
        xxfree(ptr[i]);
    }
}
