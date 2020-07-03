#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define NMEMB   10
#define SIZE    128

TEST_CASE("calloc", "[slimguard]") {
    void *ptr = xxcalloc(NMEMB, SIZE);
    REQUIRE(ptr);
    memset(ptr, 0x0, SIZE*NMEMB);
    xxfree(ptr);
}
