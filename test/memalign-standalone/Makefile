CC=gcc
CFLAGS=-g
LDFLAGS=-lpthread
PROG=memalign
SRC=memalign.c

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: $(PROG)
	LD_PRELOAD=$(PWD)/../../libSlimGuard.so ./$(PROG)

clean:
	rm -rf *.o $(PROG)
