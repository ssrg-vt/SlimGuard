#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define NMEMB   10
#define SIZE    128

int main(int argc, char **argv) {
    void *ptr = calloc(NMEMB, SIZE);
    assert(ptr);
    memset(ptr, 0x0, SIZE*NMEMB);
    free(ptr);
    return 0;
}
