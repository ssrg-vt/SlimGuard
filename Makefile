TARGETS = libSlimGuard.so
CC = gcc
CXX = g++

CFLAGS = -Wall -pedantic -O3 -Wno-deprecated-declarations -I./include

CFLAGS += -DGUARDPAGE
CFLAGS += -DRELEASE_MEM
# CFLAGS += -DDESTROY_ON_FREE
CFLAGS += -DUSE_CANARY
# CFLAGS += -DDEBUG


all: $(TARGETS)

libSlimGuard.so:
	$(CC) $(CFLAGS) -o $@ src/gnuwrapper.cpp $(wildcard src/*.c) -shared -fPIC $(LDFLAGS)

clean:
	$(RM) $(TARGETS) *.o

.PHONY: all clean
