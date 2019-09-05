#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "slimguard-large.h"

TEST_CASE("test large-obj allocation API", "[slimguard large obj]")
{
    uint8_t iter = 100;
    void *p[iter];

    for (uint32_t i = 0; i < iter; i++) {
        p[i] = xxmalloc_large(1<<20);
    }


    for (uint32_t i = 0; i < iter; i++) {
        REQUIRE(in_list(p[i]));
        //REQUIRE(((large_obj_t *)p[i])->align_size == (1<<20));
        if(xxfree_large(p[i]) == -1){
            printf("error %d %p\n", i, p[i]);
        }
    }

}

