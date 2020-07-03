#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../include/slimguard.h"
#include "../include/slimguard-large.h"
#include "../include/sll.h"

#define ALLOC_NUM   10000

TEST_CASE("memory-content", "[slimguard]") {
    uint64_t **ptr = (uint64_t **)xxmalloc(ALLOC_NUM * sizeof(uint64_t *));
    REQUIRE(ptr);

    for(int i=0; i<ALLOC_NUM; i++) {
        ptr[i] = (uint64_t *)xxmalloc(sizeof(uint64_t));
        REQUIRE(ptr[i]);
        *(ptr[i]) = 0xDEADBEEFDEADBEEF;
    }

    for(int i=0; i<ALLOC_NUM; i++) {
        REQUIRE(*(ptr[i]) == 0xDEADBEEFDEADBEEF);
        xxfree(ptr[i]);
    }

    xxfree(ptr);
}
