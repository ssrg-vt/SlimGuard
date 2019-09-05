## SlimGuard: A Secure and Memory-Efficient Heap Allocator

SlimGuard is a secure dynamic memory allocator whose design is driven by memory
efficiency. We redesign the security features of state-of-the-art allocators
with memory efficiency in mind. SlimGuard protects against widespread heap-related
attacks such as overflows, over-reads, double/invalid free, and use-after-free.
Among other features, SlimGuard uses an efficient fine-grain size classes indexing
mechanism and implements a novel dynamic canary scheme optimized for memory
overhead reduction.

#### Build SlimGuard

SlimGuard relys on gcc (v5.4.0.), it is built and run on Ubuntu 16.04.6 LTS (GNU/Linux 4.4.0-131-generic x86_64)

To build SlimGuard, run ```make``` in ```/path/to/libSlimGuard```

To use SlimGuard, you can 
1. link __libSlimGuard__ to your executable

2. setting ```LD_PRELOAD``` as an enviromment variable using 
```bash
export LD_PRELOAD=/path/to/libSlimGuard.so
```

3. simply run as 
```
LD_PRELOAD=/path/to/libSlimGuard.so /your/app
```

