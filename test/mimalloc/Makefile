CC=gcc
CFLAGS=-g
LDFLAGS=-lpthread
PROG=test-stress
SRC=test-stress.c

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: $(PROG)
	LD_PRELOAD=$(PWD)/../../libSlimGuard.so ./$(PROG)

clean:
	rm -rf *.o $(PROG)
