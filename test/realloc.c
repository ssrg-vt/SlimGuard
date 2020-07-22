#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define OLD_SIZE    128
#define NEW_SIZE    2048

#if OLD_SIZE > NEW_SIZE
#error "OLD_SIZE must be < NEW_SIZE"
#endif

TEST_CASE("realloc", "[slimguard]") {
    void *ptr = xxmalloc(OLD_SIZE);
    REQUIRE(ptr);
    memset(ptr, 0x0, OLD_SIZE);

    ptr = xxrealloc(ptr, NEW_SIZE);
    memset((char *)ptr + OLD_SIZE, 0x0, NEW_SIZE - OLD_SIZE);

    for(int i=0; i<NEW_SIZE; i++) {
        int val = *((char *)ptr+i);
        REQUIRE(val == 0);
    }

    xxfree(ptr);
}
