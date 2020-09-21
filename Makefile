TARGETS = libSlimGuard.so
CC = gcc
CXX = g++
STANDALONE=slimguard-standalone

CFLAGS = -Wall -pedantic -O3 -Wno-deprecated-declarations -I./include

CFLAGS += -DGUARDPAGE
CFLAGS += -DRELEASE_MEM
# CFLAGS += -DDESTROY_ON_FREE
CFLAGS += -DUSE_CANARY
# CFLAGS += -DDEBUG


all: $(TARGETS)

libSlimGuard.so:
	$(CC) $(CFLAGS) -o $@ src/gnuwrapper.cpp $(wildcard src/*.c) -shared -fPIC $(LDFLAGS)

standalone:
	rm -rf $(STANDALONE).c
	sed "s/#include \"/\/\/ #include \"/" include/*.h >> $(STANDALONE).c
	sed "s/#include \"/\/\/ #include \"/" src/slimguard.c >> $(STANDALONE).c
	sed "s/#include \"/\/\/ #include \"/" src/slimguard-large.c >> $(STANDALONE).c
	sed "s/#include \"/\/\/ #include \"/" src/slimguard-mmap.c >> $(STANDALONE).c
	sed "s/#include \"/\/\/ #include \"/" src/sll.c >> $(STANDALONE).c
	echo "" >> $(STANDALONE).c
	echo "	int main(int argc, char **argv) {" >> $(STANDALONE).c
	echo "		int num = 1000000;" >> $(STANDALONE).c
	echo "		void *ptr[num];" >> $(STANDALONE).c
	echo "" >> $(STANDALONE).c
	echo "		for (int i = 0; i < num; i++) {" >>  $(STANDALONE).c
	echo "			int size = rand()%1024;" >> $(STANDALONE).c
	echo "			ptr[i] = xxmalloc(size);" >> $(STANDALONE).c
	echo "			assert(ptr[i]);" >> $(STANDALONE).c
	echo "			memset(ptr[i], 0x0, size);" >> $(STANDALONE).c
	echo "		}" >> $(STANDALONE).c
	echo "" >> $(STANDALONE).c
	echo "	for (int i = (num-1); i >= 0; i--) {" >> $(STANDALONE).c
	echo "		xxfree(ptr[i]);" $(STANDALONE).c
	echo "	}" >> $(STANDALONE).c
	echo "" >> $(STANDALONE).c
	echo "return 0;" >> $(STANDALONE).c
	echo "}" >> $(STANDALONE).c

clean:
	$(RM) $(TARGETS) *.o

.PHONY: all clean
