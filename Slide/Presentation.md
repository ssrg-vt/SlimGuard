### Intro
Hello everyone, thanks for coming to my defense. My Thesis topic is "Design and
Implementation of a memory efficient and secure heap allocator.

### Motivation
In the past decade, "heap" have become a more and more popular word in the
vulnerability database. In this figure, it showed the number of heap-related
vulnerability entries since 2010. As you can see, the number has increased a
lot since 2016. 

What are these vulnerabilities?
These includes buffer overflows/overreads, use-after-free, double/invalid
frees.

### Heap Vulnerabilities
A buffer overflow frequently happened in memory unsafe languages such as C/C++.
For example, assume the user is trying to allocate 8 byte of memory, and
somehow, there isn't any boundary check for this allocation. So the user can
write 10 bytes to the buffer. This allocation overflowed two bytes, which may
potentally overwrite the next buffer.

Use-after-free is the application is trying to use a free'd buffer.

Double free is the application is trying to free a free'd buffer again.

heap vulnerabilities can lead to serious problems such as data/metadata
corruption, control flow hijacking, information leaks, etc.

### Related works

One solution to solve these heap related vulnerabilties is to use a secure heap
allocator also known as a secure malloc algorithm

OpenBSD allocators provide some heap protections such as segratagation of
metadata, randomized allocation, it is not very secure 

Early works such as DieHarder provided some new techniques such as
over-provisioning.  but it is slow, which means the performance overhead is
huge.

Recent works FreeGuard and Guarder provide high performanace, secure memory
allocator.  however its memory overhead is more than 2x in a lot of sceriano

But they all suffer one big problem: Memory overhead. Existing secure allocator
are either not secure or memory efficient.

### proposal

Here we propose slimguard! A secure memory allocator, not only has negligible
performance overhead and competetive security guarantees, but alsoo more
importantly it is memory efficient. 

### Design

First of, let me talk about general non-secure designs.

#### Freelist
There are two different types of memory allocators. The first one is free-list
based allocator. GlibC's default allocator is free-list based allocator. It rounds 
the size of the object to 8 or 16 bytes depending on the architecture. and it 
stores the size right before the heap object. 

For instance, in this figure, if the application is trying to allocate a
buffer, it will give the return addr, shown here. After this ret addr, it is
the buffer space, before the return addr, it is the metadata. 

As you can see the free-list based allocator is not very secure, because it
is desgined for performance and stability. 

#### Bibop
Another type of heap allocator is Bibop style. BIBOP means Big Bag of Pages. 
OpenBSD Guarder and other secure memory allcoators are designed this way. 
It has a power-of-2 class size, meaning each bag has the same slot that is 
power-of-2. For example, if the user is trying to allocate 4098 bytes, it will
come here. But what if you want to allocate 2048+1 bytes? Since there is only 
2048 and 4096 size classes, so the allocator has to sacrifice 2047 bytes and 
give it a 4096 byte slot. This s a huge waste of memory.

#### Slimguard
On the other hand, slimguard is designed with fine-grainede size classes. it
has subdivisions between the power-of-2 size classes. For example, if the user
is allocating 129 bytes. Instead of going to 256 byte slots, it will go to the
size-class with an index of 17. This fine-grained classes can save a lot of
memoreis at runtime.  


#### Malloc path

When we have a malloc, we will use the size x to calculate the size class index
I.  In the class size index I, we will randomly find a slot in the free-listi, 
write the corresponding metadata and return the the address to the user.

#### free path

When we free an object, slimguard will find the class size index I, and write
the corresponding metadata.

#### Multi-threading support
Slimguard is using a fine-grained mutex lock to secure the critical sections,
the buffer and the metadata. So a lot of computations can be done in parallel.

#### Binary Compatibailty
Slimguard supports all of the memory allocation APIs includes malloc, free,
realloc, memalign, and it is designed as a dynamic library, so it can be easily
linked to an existing bianry.

### Security features
Next, lets talk about the security features in SlimGuard. 

#### Randomization Allocatoin

In order to make it harder for the attacker to guess what is the next return address
we have to make it as random as possible. In order to achieve this goal, we build a 
very large array, it contains a lot of poitners(2^10) that can be used to serve a 
memory allocation. Everytime a pointer is randomly chosen, slimguard will fill
the next possible slot to this free array in order to make sure it always have 
enough free slot to use.

#### Metadata segrafation

To prevent the metadata to be overwritten, Slimguard separated the metadata and
the objects itself. As you can see, Part B and Part C are separated in two 
non-contingenious memory area. This will make sure that the metadata cannot be
overwritten.

#### Dynamic Canary

In order to detect the buffer overflows, SlimGuard introduced a new technique: 
Dynamic Canary. This is a hased value based on the return address of each buffer.
Since the return address differs from on and the other, so techniqually, each 
slot will have a different value of canary. In this way, SlimGuard does not 
have to store the canary value, which makes it more secure comparing to the others.
Results also shown the overhead comes from this part is negligible.

#### Guard Oages

Guard pages is another way to detch buffer overflows. It can immediately detect buffer
overflow since each guard page here is unmapped pages. If an overwrite writes the 
guard page, a seg fault will immmediately detect the overflow. Since the cost of
deploying guard pages are huge. So we make it fully on demand. i.e. we do not 
pre-set guard pages. This will shorten the initialization time for the allocator.

#### Other security features

destroy onfree 

delayed memory reuse

### results
top: memory overhead

bottom performance

x axis, y axis, different colors

multi-threading up to 64 threads

security features
Lastly, let's see how slimguard detects real-world bugs:
1. gzip and ncompress contains buffer overflow due to strcpy
 without proper boundary checks. SlimGuards can detect them 
when it checks the canray value, so it reports the overflow and
halt the program.

2. ed and ImageMagic contains double or invalid frees, which can be 
immeditately detected by slimguard when it happens.

### Conclusion

