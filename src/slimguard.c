/*  slimguard.c
 *  SlimGuard
 *  Copyright (c) 2019, Beichen Liu, Virginia Tech
 *  All rights reserved
 */

#include "slimguard.h"
#include "slimguard-large.h"
#include "sll.h"
#include "debug.h"
#include "slimguard-mmap.h"

#include <assert.h>
#include <pthread.h>

const int tab64[64] = {
  63,  0, 58,  1, 59, 47, 53,  2,
  60, 39, 48, 27, 54, 33, 42,  3,
  61, 51, 37, 40, 49, 18, 28, 20,
  55, 30, 34, 11, 43, 14, 22,  4,
  62, 57, 46, 52, 38, 26, 32, 41,
  50, 36, 17, 19, 29, 10, 13, 21,
  56, 45, 25, 31, 35, 16,  9, 12,
  44, 24, 15,  8, 23,  7,  6,  5
};

typedef struct cls_t{
  void* bucket[BKT];        // array for randomization and OP entropy
  void* start;              // start address of a size class
  void* stop;               // stop address of a size class
  void* current;            // bumper pointer
  void* guardpage;          // last guard page
  sll_t* head;              // head of the free list
  uint64_t bitmap[ELE>>6];  // bitmap
  pthread_mutex_t lock;
  uint32_t size;            // size of current sizeclass
} cls_t;

#ifdef RELEASE_MEM
uint16_t *page_counter[INDEX];
#endif

cls_t Class[INDEX];
uint32_t seed = 0;
pthread_mutex_t global_lock;

enum pool_state{null, init} STATE;

/* convert a size to its corresponding size Class */
uint8_t sz2cls(uint32_t sz) {
    if (sz < 128) {
        return ((sz>>(SLI_LOG2-1)) + ((sz&0x7) ? 1:0 ));
    } else {
        return ((((log2_64(sz)-MIN_EXP) << SLI_LOG2) |
                 ((sz>> (log2_64(sz)-SLI_LOG2))& ~(1 << SLI_LOG2)))
                 + ((((1<<(log2_64(sz)-SLI_LOG2)) - 1) & sz) ? 1:0 ));
    }
}

/* Find the size given a index */
uint32_t cls2sz(uint16_t cls) {
    if (cls < 16) {
        return (cls << (SLI_LOG2-1));
    } else {
        uint8_t shift = (cls >> SLI_LOG2) + 6;
        return ((1U<<shift) + ((cls & ((1U<< SLI_LOG2)-1))<<(shift-SLI_LOG2)));
    }
}

/* round the sizeup with the aligment */
uint32_t round_sz(uint32_t sz) {
    return ((sz + ALIGN) &(~ALIGN));
}

/* get size byte of virtual memory */
void* get_mem(uint64_t size) {
    void* ret = NULL;

    ret = slimguard_mmap(size);
    Debug("ret %p %lu\n", ret, size);

    if (ret == NULL) {
        Error("cannot mmap for size %lu\n", size);
        exit(-1);
    }

    return ret;
}

/* SlimGuard initialization */
void init_bibop() {
    srand(time(NULL));

    Debug("Entropy %d\n", ETP);

#ifdef USE_CANARY
    seed = time(NULL) % SEED_MAX;
#endif

    for (int i = 0; i < INDEX; i++) {
        Class[i].start = NULL;
    }

    STATE = init;
}

/* Initialize each size class, fill in per size class data structure */
void init_bucket(uint8_t index) {
    if (Class[index].start == NULL) {
        void* addr = get_mem(BUCKET_SIZE); // start address of a bucket

        Class[index].head = NULL; // head of the sll contains free pointers
        Class[index].start = addr; // start address of the current bucket
        Class[index].current = Class[index].start; // bumper pointer
        Class[index].stop = (void *)((uint64_t)addr + BUCKET_SIZE); // upper
                                                                    // bound
        Class[index].size = cls2sz(index); // the size for current bucket

        for (int i = 0; i < BKT; i++) {
            Class[index].bucket[i] = Class[index].current;
            Class[index].current = (void *) ((uint64_t)Class[index].current +
                    (uint64_t)Class[index].size);
        }

        Debug("BKT %d size: %d, start: %p\n", BKT, Class[index].size,
                Class[index].start);

#ifdef RELEASE_MEM
        page_counter[index] = get_mem(4<<20);
#endif

        Debug("index: %u size: %u %p %p %p\n", index, Class[index].size,
                Class[index].start, Class[index].stop, Class[index].current);

#ifdef GUARDPAGE
        void* next_page = (void *)((((uint64_t)Class[index].current >> 12)
                    + 1 ) <<12);
        if (mprotect(next_page, PAGE_SIZE-1, PROT_NONE) == 0) {
            Class[index].guardpage = next_page;
        } else {
            perror("mprotect");
            Error("[%x] %p %p %p\n", index, Class[index].start, next_page,
                    Class[index].stop);
            exit(-1);
        }
#else
        Class[index].guardpage = (uint64_t *)-1;
#endif
    }
}

/* get next slot to fill in the bucket */
void *get_next(uint8_t index){
    void* ret = Class[index].current;

#ifdef GUARDPAGE
    if ((ret >= Class[index].guardpage) |
        ((uint64_t)ret + cls2sz(index) >= (uint64_t)Class[index].guardpage)){
        ret = (void *)((((uint64_t )Class[index].guardpage >> 12) + 1) << 12);
    }
#endif

    Class[index].current = (void *)((uint64_t)ret + (uint64_t)Class[index].size);

    if(Class[index].current >= Class[index].stop){
        Error("not enough mem %u\n", Class[index].size);
        exit(-1);
    }

#ifdef GUARDPAGE
    if( (ret > Class[index].guardpage) |
        ((uint64_t)ret + Class[index].size >=
         (uint64_t)Class[index].guardpage)) {
        void * next_guard = (void *)((((uint64_t)Class[index].current >> 12) +
                    GP ) <<12);

        if (mprotect(next_guard, PAGE_SIZE-1, PROT_NONE) == 0) {
            Class[index].guardpage = next_guard;
        } else {
            perror("mprotect");
            exit(-1);
        }
    }
#endif

    return ret;
}

/* select a random object in a bucket */
void* get_random_obj(uint8_t index) {
    uint16_t i = rand() % BKT;
    void *ret = Class[index].bucket[i];

    if (!ret) {
        Error("%p %p\n", Class[index].current, Class[index].start);
        Error("Cannot get next object for class size(%x)\n\n", index);
        abort();
    }

    Class[index].bucket[i] = get_next(index);

    return ret;
}

/* Hash a pointer to get canary value */
char HashPointer(void* ptr) {
    long long Value = (long long)ptr;

    Value = ~Value + (Value << 15);
    Value = Value ^ (Value >> 12);
    Value = Value + (Value << 2);
    Value = Value ^ (Value >> 4);
    Value = Value * seed;
    Value = Value ^ (Value >> 16);

    return (char)Value;
}

/* put the canary to the end of each block, set it to MAGIC NUMBER */
void set_canary(void * ptr, uint8_t index) {
    char* end = (char *)((unsigned char *)ptr + Class[index].size - 1);
    *end = HashPointer(ptr);

#ifdef DEBUG
    Canary("[%p] %p %x %u\n", ptr, end, *end, index);
#endif
}

/* check the canary value */
void get_canary(void *ptr, uint8_t index) {
    char *end = (char *)((unsigned char *) ptr + Class[index].size - 1);

#ifdef DEBUG
    Canary("[%p] %p %x\n", ptr, end, *end);
#endif

    if(*end != HashPointer(ptr)) {
        Error("buffer overflow occured at %p, exiting now\n", ptr);
        Error("%x\n", HashPointer(ptr));
        Error("[%p] %p %x %u %u\n", ptr, end, *end, Class[index].size, index);
        exit(-1);
    }
}

/* mark the bit map as "used" */
void mark_used(void *ptr, uint8_t index) {
    uint64_t i = (uint64_t)((char*)ptr -
            (char*)Class[index].start) / Class[index].size;
    uint32_t bitmap_index = i >> 6;
    uint8_t shift = i % (1<<6);

    Class[index].bitmap[bitmap_index] |= (1UL << shift);
    Debug("i: %x bit: %u shift %u bitmap: %x\n", i, bitmap_index, shift,
            Class[index].bitmap[bitmap_index]);
}

/* mark the bit map as "free" and check for potential vulunerabilities */
void mark_free(void *ptr, uint8_t index) {
    uint64_t i = (uint64_t)((char*)ptr - (char*)Class[index].start) / Class[index].size;
    uint32_t bitmap_index = i >> 6;
    uint8_t shift = i % (1<<6);

    if((Class[index].bitmap[bitmap_index] & (1UL << shift)) == 0) {
        Error("Double Free or Invalid Free @ %p\n", ptr);
        abort();
    }

    Class[index].bitmap[bitmap_index] &= ~(1UL << shift);
    Debug("i: %x bit: %u shift %u bitmap: %x\n", i, bitmap_index, shift,
            Class[index].bitmap[bitmap_index]);
}

#ifdef RELEASE_MEM
/*
 * if release_mem is defined, we will setup an array of counter,
 * each counter stores the number of object in a page, once this
 * number becomes 0, we will release the memory back to Operating System
 */
void increment_pc(void *ptr, uint8_t index) {
    uint64_t curr_page = ((uint64_t )ptr >> 12) <<12;
    uint16_t obj_size = Class[index].size;
    uint16_t *pc, pc_index;

    do {
        pc_index = (curr_page - (uint64_t)Class[index].start)/PAGE_SIZE;
        pc = (uint16_t *)((uint64_t)page_counter[index] + pc_index*sizeof(uint16_t));
        *pc = *pc + 1;
        curr_page += PAGE_SIZE;
    } while((uint64_t) ptr+obj_size >= curr_page);

  //Debug("[%u]: %p \n", index, ptr);
}

void decrement_pc(void *ptr, uint8_t index) {
    uint64_t curr_page = ((uint64_t )ptr >> 12) <<12;
    uint16_t obj_size = Class[index].size;
    uint16_t *pc, pc_index;

    do {
        pc_index = (curr_page- (uint64_t)Class[index].start)/PAGE_SIZE;
        pc = (uint16_t *)((uint64_t)page_counter[index] + pc_index*sizeof(uint16_t));
        *pc = *pc - 1;

        if (*pc == 0)
            madvise((void *)curr_page, PAGE_SIZE-1, MADV_DONTNEED);

        curr_page += PAGE_SIZE;
    } while((uint64_t) ptr + obj_size >= curr_page);
}
#endif

/* given a pointer find the corresponding size class */
uint16_t find_sz_cls(void *ptr) {
    static __thread int last_index = 0;

    for(int i=last_index; i<INDEX; ++i) {
        if(((uint64_t)Class[i].start <= (uint64_t)ptr) &
           ((uint64_t)Class[i].stop > (uint64_t)ptr)){
            last_index = i;

            return i;
        }
    }

    for(int i=0; i<last_index; ++i) {
        if(((uint64_t)Class[i].start <= (uint64_t)ptr) &
           ((uint64_t)Class[i].stop > (uint64_t)ptr)){
            last_index = i;

            return i;
        }
    }

    return 255; // TODO pierre fix this
}

/* a fast way to get the integer part of log2 */
int log2_64 (uint64_t value) {
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  value |= value >> 32;

  return tab64[((uint64_t)((value - (value >> 1))*0x07EDD5E59A4E28C2)) >> 58];
}

/* SlimGuard malloc algorithm */
void* xxmalloc(size_t sz) {
#ifdef USE_CANARY
    sz++;
#endif

    uint64_t need = 0;
    void *ret = NULL;
    uint8_t index = 255;
    need = round_sz(sz);

    if (need >= (1 << 17)) {
        Debug("sz %lu\n", need);
        ret = xxmalloc_large(need);
    } else {
        if (STATE == null) {
            /* Lock here */
            pthread_mutex_lock(&global_lock);
            init_bibop();
            pthread_mutex_unlock(&global_lock);
            /* Lock end */
        }

        index = sz2cls(need);

        if (index == 255) {
            perror("sz2cls");  // TODO pierre fix this
            return NULL;
        }

    /* Lock here */
    pthread_mutex_lock(&(Class[index].lock));

    if (Class[index].start == NULL)
        init_bucket(index);

    if (Class[index].head && Class[index].head->next) {
        ret = Class[index].head;
        Class[index].head = remove_head(Class[index].head);
    } else {
        ret = get_random_obj(index);
    }

    pthread_mutex_unlock(&(Class[index].lock));
    /* Lock end */

#ifdef USE_CANARY
    set_canary(ret, index);
#endif

#ifdef RELEASE_MEM
    increment_pc(ret, index);
#endif

    /* Lock Here */
    pthread_mutex_lock(&(Class[index].lock));
    mark_used(ret, index);
    pthread_mutex_unlock(&(Class[index].lock));
    /* Lock End */
  }

  return ret;
}
/* SlimGuard free */
void xxfree(void *ptr) {
    uint8_t index = 255;

    if (ptr == NULL)
        return;

    index = find_sz_cls(ptr);

    if (index == 255) {
        int i = xxfree_large(ptr);

        if (i == 1) {
            return;
        } else if (i == -1) {
            Error("invalid free/double free %p\n", ptr);
            abort();
            return;
        }
    }

#ifdef USE_CANARY
    get_canary(ptr, index);
#endif

#ifdef RELEASE_MEM
    decrement_pc(ptr, index);
#endif

#ifdef DESTROY_ON_FREE
    memset(ptr, 0, Class[index].size);
#endif

    /* Lock here */
    pthread_mutex_lock(&(Class[index].lock));
    Class[index].head = add_head((sll_t *)ptr, Class[index].head);
    mark_free(ptr, index);
    pthread_mutex_unlock(&(Class[index].lock));
    /* Lock end */
}

void* xxrealloc(void *ptr, size_t size) {
#ifdef USE_CANARY
    size++;
#endif
    uint8_t index, index2;
    void* ret = ptr;

    if(ptr == NULL)
        return xxmalloc(size);

    if(size == 0) {
        xxfree(ptr);
        return NULL;
    }

    index = find_sz_cls(ptr); // old size
    index2 = sz2cls(size); // new size

    if( index2 >= index  ) {
        ret = xxmalloc(size);
        memcpy(ret, ptr, Class[index].size-1);
        xxfree(ptr);
    }

    return ret;
}

void* xxmemalign(size_t alignment, size_t size) {
    /* Here we used a small trick, Our power of 2 size classesrealigned,
     * if alignment is larger than size, we just malloc alignment to give
     * it an address,
     * if alignment is smaller than size, we round the size to the next
     * power of 2.
     */

    if(alignment &(alignment-1)){
        Error("%lu is not a valid alignment, %s fails...\n", alignment, __func__);
        exit(-1);
    }

#ifdef USE_CANARY
    size++;
#endif

    if(alignment >= size) {
        return xxmalloc(alignment-1);
    } else {
#ifdef USE_CANARY
        uint64_t need = (1 << ((uint8_t)log2_64(size) + 1))-1;
#else
        uint64_t need = (1 << ((uint8_t)log2_64(size) + 1));
#endif
        return xxmalloc(need);
    }
}

/* high level SlimGuard API that is called by gnuwrapper */
void* slimguard_malloc(size_t size) {
  return xxmalloc(size);
}

void slimguard_free(void *ptr) {
  xxfree(ptr);
}

void* slimguard_realloc(void *ptr, size_t size) {
  return xxrealloc(ptr, size);
}

void* slimguard_memalign(size_t alignment, size_t size) {
  return xxmemalign(alignment, size);
}

