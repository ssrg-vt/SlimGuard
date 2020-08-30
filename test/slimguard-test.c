#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

TEST_CASE("bitmap", "[slimguard]")
{
    int num = 1000000;
    void *ptr[num];

    for (int i = 0; i < num; i++) {
        int size = rand()%1024;
        ptr[i] = xxmalloc(size);
        REQUIRE(ptr[i]);
        memset(ptr[i], 0x0, size);
    }

    for (int i = (num-1); i >= 0; i--) {
        xxfree(ptr[i]);
    }
}
