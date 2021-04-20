cmake_minimum_required(VERSION 3.7.2)  # CMake version check
project(SlimGuard)

include_directories(../include)

set(CMAKE_C_COMPILER g++)

set(debug off)
set(canary on)
set(release-memory on)
set(guardpage on)
set(destroy-on-free on)

include_directories(../include/)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-deprecated-declarations -O0 -g")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")

if(debug)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -DDEBUG")
endif(debug)

if(canary)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_CANARY")
endif(canary)

if(release-memory)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DRELEASE_MEM")
endif(release-memory)

if(guardpage)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGUARDPAGE")
endif(guardpage)

if(destroy-on-free)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDESTROY_ON_FREE")
endif(destroy-on-free)


set(debug ../include/debug.h)
set(catch catch.hpp)

set(slimguard-large ${debug} ../test/slimguard-large-test.c)
set(memalign ${debug} ../test/memalign.c)
set(memalign-free ${debug} ../test/memalign-free.c)
set(large-alloc ${debug} ../test/large-alloc.c)
set(memory-content ${debug} ../test/memory-content.c)
set(slimguard-test ${debug} ../test/slimguard-test.c)
set(malloc-test ${debug} ../test/malloc-test.c)
set(mimalloc-test ${debug} ../test/mimalloc-test.c)
set(memory-content ${debug} ../test/memory-content.c)
set(calloc ${debug} ../test/calloc.c)
set(realloc ${debug} ../test/realloc.c)
set(very-large ${debug} ../test/very-large.c)
set(slimguard-base ${debug} ../include/slimguard.h
                            ../src/slimguard.c
                            ../src/slimguard-large.c
                            ../src/sll.c
                            ../include/slimguard-mmap.h
                            ../src/slimguard-mmap.c
                            ../src/slimguard-large.c
                            ../include/slimguard-large.h)

enable_testing()

add_executable(slimguard-large-test ${slimguard-large} ${slimguard-base} ${catch})
add_executable(slimguard-test ${slimguard-test} ${slimguard-base} ${catch})
add_executable(memalign ${memalign} ${slimguard-base} ${catch})
add_executable(memalign-free ${memalign-free} ${slimguard-base} ${catch})
add_executable(large-alloc ${large-alloc} ${slimguard-base} ${catch})
add_executable(memory-content ${memory-content} ${slimguard-base} ${catch})
add_executable(malloc-test ${malloc-test} ${slimguard-base} ${catch})
add_executable(mimalloc-test ${mimalloc-test} ${slimguard-base} ${catch})
add_executable(calloc ${calloc} ${slimguard-base} ${catch})
add_executable(realloc ${realloc} ${slimguard-base} ${catch})
add_executable(very-large ${very-large} ${slimguard-base} ${catch})

add_test(slimguard-large-test slimguard-large-test)
add_test(slimguard-test slimguard-test)
add_test(memalign memalign)
add_test(memalign-free memalign-free)
add_test(large-alloc large-alloc)
add_test(memory-content memory-content)
add_test(malloc-test malloc-test)
add_test(mimalloc-test mimalloc-test)
add_test(calloc calloc)
add_test(realloc realloc)
add_test(very-large very-large)
