cmake_minimum_required(VERSION 3.7.2)  # CMake version check
project(SlimGuard)

include_directories(../include)

set(CMAKE_C_COMPILER g++)

set(debug off)
set(canary on)
set(release-memory off)
set(guardpage off)
set(destroy-on-free off)

include_directories(../include/)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated-declarations -O2")

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

set(sll ${debug} ../include/sll.h ../src/sll.c ../test/sll-test.c)
set(slimguard-large ${debug} ../include/slimguard-large.h ../src/slimguard-large.c ../test/slimguard-large-test.c)
set(slimguard ${debug} ../include/slimguard.h
                       ../src/slimguard.c
                       ../src/slimguard-large.c
                       ../src/sll.c
                       ../test/slimguard-test.c)
enable_testing()

add_executable(sll-test ${sll} ${catch})
add_executable(slimguard-large-test ${slimguard-large} ${catch})
add_executable(slimguard-test ${slimguard} ${catch})

add_test(sll-test sll-test)
add_test(slimguard-large-test slimguard-large-test)
add_test(slimguard-test slimguard-test)
