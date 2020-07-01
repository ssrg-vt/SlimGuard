#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

int main(int argc, char **argv) {
    void *ptr = memalign(128, 128);
    memset(ptr, 0x0, 128);
    free(ptr);
}
