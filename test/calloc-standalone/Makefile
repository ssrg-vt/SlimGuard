CC=gcc
CFLAGS=-g
LDFLAGS=-lpthread
PROG=calloc-standalone
SRC=calloc-standalone.c

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: $(PROG)
	LD_PRELOAD=$(PWD)/../../libSlimGuard.so ./$(PROG) 100000 1048576

clean:
	rm -rf *.o $(PROG)
