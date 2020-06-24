cmake_minimum_required(VERSION 3.7.2)  # CMake version check
project(SlimGuard)

set(debug off)
set(canary on)
set(release-memory on)
set(guardpage on)
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

set(SlimGuard-src
    ../include/debug.h
    ../include/slimguard.h slimguard.c
    ../include/slimguard-large.h slimguard-large.c
    ../include/slimguard-mmap.h slimguard-mmap.c
    ../include/sll.h sll.c
    gnuwrapper.cpp)

add_library(SlimGuard SHARED ${SlimGuard-src})
