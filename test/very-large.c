#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

#define MAX_ALIGNMENT   (1024*1024)
#define ITERATIONS      500
#define VERBOSE         0

void* p[256];
uintptr_t buf[256];

TEST_CASE("very_large", "[slimguard]")
{
    p[0] = xxmalloc(-8);
    REQUIRE(!p[0]);
    fprintf(stderr, "%p\n", p[0]);
}
