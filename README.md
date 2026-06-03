
This is an implementation of malloc, realloc, free and few other functions.


As required by the subject, the only permitted system calls are `mmap()`, `munmap()`, `getpagesize()`/`sysconf()`, and `getrlimit()`, along with libpthread functions and a single global variable. Memory zones must contain at least 100 allocations, with 3 different heap sizes: TINY: 1 to n bytes, stored in N-byte zones, SMALL: n+1 to m bytes, stored in M-byte zones  and LARGE: m+1 and above, handled directly with `mmap()`

## Zone Sizes

`TINY`: 4 * PAGE_SIZE (16,384 bytes)
Yields 127 blocks of 128 bytes each (112 bytes data + 16 bytes chunk header).

`SMALL`: 32 * PAGE_SIZE (131,072 bytes)
Yields 127 blocks of 1,024 bytes each (1,008 bytes data + 16 bytes chunk header), plus one remaining block of 960 bytes.

`LARGE`: size of request + sizeof(t_heap)

The header is included within these zone sizes for TINY and SMALL. Allocating `ZONE_SIZE + header` via `mmap()` would round up to the next page multiple, wasting an extra page so it is consumed from the zone size.


## Memory Layout

**Symbol Map**


[HH] - Heap Header - (t_heap)
 - Contains padding, total size, n of alloced chunks, ptr to cis memory and ptrs to next and prev heaps.

[CH] - Chunk Header - (t_chunk)
 - Contains prev_size and size, and *next and *prev when free.
 - [CH(cis)]   - Initial chunk of leftover free memory.
 - [CHF(free)] - Previously alloced chunk, now free.
 - [CH(used)]  - Chunk used by user.

[...] - User data - (void *)


**Pages are mapped as follows:**

On initial malloc:
```
[[HH][CH(cis)][.....................................................]]
```

After an allocation:

```
[[HH][CH(used)][...][CH(cis)][.......................................]]
```

After several allocations and frees:

```
[[HH][CH(used)][...][CHF(free)][...][CH(used)][...][CH(cis)][.........]]
```


## Data Structures

```c
typedef struct s_heap {

    size_t        padding1, padding2, padding3; // explicit padding
    size_t        alloc_chunks;     // total allocated chunks, used to detect when to release memory
    size_t        total_size;       // total bytes available from the start (unchanged)
    t_chunk       *free_cis_start;  // pointer to the top of untouched (CIS) memory
    struct s_heap *next;            // next heap of the same type
    struct s_heap *prev;            // previous heap of the same type

} t_heap;
```

```c
typedef struct s_chunk {

    size_t         prev_size; // size of previous chunk if it is free, otherwise 0
    size_t         size;      // data size; the 2 least significant bits indicate IN_USE or CIS
    struct s_chunk *next;     // used by bins when freed; serves as data otherwise
    struct s_chunk *prev;     // used by bins when freed; serves as data otherwise

} t_chunk;
```

```c
typedef struct s_arena {
    pthread_mutex_t lock;         // mutex for thread-safe access
    MALLOC_OPS      OPS;          // flags set via mallopt()
    t_heap          *tiny;        // head of TINY heap l-list
    t_heap          *small;       // head of SMALL heap l-list
    t_heap          *large;       // head of LARGE heap l-list
    t_chunk         *fastbin[7];  // LIFO linked lists for TINY freed chunks
    t_chunk         *smallbin[56];// FIFO circular linked lists for SMALL freed chunks
    t_heap          *tiny_cache;  // recently freed TINY heap, held before munmap()
    t_heap          *small_cache; // recently freed SMALL heap, held before munmap()
} t_arena;
```

```c
typedef struct MALLOC_OPS
{
	uint8_t			SHOW_INFO;  // show all memory in heap
	uint8_t			PERTURB;    // fill x and ~x bytes on alloc and free
	uint8_t			CHECK;      // level of warning to throw (print/abort/silent)
	uint8_t			ZERO;       // fill with 0s on alloc

} MALLOC_OPS;
```

## Bins & Cache

`Fastbins` (TINY): An array of 7 pointers to singly-linked lists. Freed TINY chunks are pushed in LIFO order. Sizes range from 16 to 112 bytes in 16-byte increments.

`Smallbins` (SMALL): An array of 56 pointers to circular doubly-linked lists. The tail is always popped to ensure the oldest freed chunk is reused first (FIFO).

`Tiny/Small Cache` Pointers to a previously completely freed heap. Used to give one last chance to heap before munmapping it.


## Public Interface

Each function has a Public interface which does some checks for early exit and locks the mutex before calling internal logic. Valgrind macros are included so that tools track aligned data sizes rather than raw mapped pages.

Example:

```c
__attribute__((visibility("default")))
void *malloc(size_t size) {

    pthread_mutex_lock(&g_arena.lock);

    void *ptr = malloc_internal(size);

    if (ptr != NULL)
        VALGRIND_MALLOCLIKE_BLOCK(ptr, ALIGN(size), 0, 0);

    pthread_mutex_unlock(&g_arena.lock);
    return ptr;
}
```

## Function Logic Walkthrough

```
void *s = malloc(16);
```

 - Align the requested size to a multiple of 16.
 - Check the appropriate fastbin or smallbin for a free chunk. If found, pop it and return a pointer to its data.
 - If no bin match, scan all heaps of the appropriate type for remaining CIS memory and split from it.
 - If no CIS memory is available, obtain a new heap — from the cache if one exists, otherwise via `mmap()` — then split and return.

```
free(s);
```

 - If `ptr` is NULL, return immediately.
 - If `ptr` is misaligned, print an error and return.
 - Iterate through heaps of the appropriate type to confirm the pointer belongs to one and that it is not already freed or CIS memory.
 - Place the chunk in the relevant fastbin or smallbin.
 - For smallbin placements, check adjacent chunks. If either neighbor is also free, attempt coalescing (merging), then re-insert into the appropriate bin.
 - If the heap is now entirely empty, remove it from the linked list. Place it in the cache if the slot is free, otherwise call `munmap()`.

The internal free also returns error codes to the public wrapper. When `MALLOC_CHECK_` is set to 3, double-frees, invalid pointers, and similar errors will print a message and/or abort.

```
char *s = realloc(s, 100);
```

 - If `ptr` is misaligned, return NULL.
 - If size would overflow, return NULL.
 - If `ptr` is NULL, behave as `malloc(size)`.
 - If `size` is 0, behave as `free(ptr)`.
 - Find the heap the chunk belongs to.
 - If the new size equals the current aligned size, return `ptr` unchanged.
 - If the new size fits within the current aligned chunk, return `ptr` unchanged.
 - If the new size belongs to a different heap type, `malloc` a new block, copy the data, and `free` the old pointer.
 - If the new size is within the same heap type, try to split the current chunk, or absorb a free right or left neighbor. Place any remainder into a bin.
 - If none of the above apply, `malloc` a new block, copy the data, and `free` the old pointer.

Note: `realloc` does not guarantee the returned pointer will be the same as the one passed in.

```
show_alloc_mem();
```

Iterates through all heaps and prints the address ranges of all IN_USE chunks.

```
show_alloc_mem_ex();
```

Same as `show_alloc_mem`, but also dumps the raw bytes and ASCII representation of each chunk's data. If `MALLOC_CHECK_ACTION_` is set to 1, the full contents of every heap are displayed, and total allocated data size and total free size.

```
mallopt(MALLOC_PERTURB_, 0xFF); / ENV variables
```

Four options can be configured either by calling `mallopt()` or by setting environment variables before launch:

 - `MALLOC_PERTURB_` (0–255) | Fill allocated memory with the given byte; fill freed memory with its bitwise complement
 - `MALLOC_ZERO` (0–1) | Fill allocated and freed memory with zero bytes
 - `MALLOC_CHECK_` (0–3) | Error reporting level: 0 = silent, 1 = print only, 2 = abort only, 3 = print and abort
 - `MALLOC_SHOW_INFO_` (0–1) | Controls the verbosity level of `show_alloc_mem_ex`

They can also be passed to mallopt() with their equivalent names:

`M_SHOW_INFO`, `M_CHECK_ACTION`, `M_PERTURB`, `M_ZERO`


## Building and usage

This produces `libft_malloc.so` and a symlink `libft_malloc_$(HOSTTYPE).so`.

```bash
git clone git@github.com:cristommasi/malloc.git
make
```

Linking library for symbols:

```bash
#include "path-to-lib-header/malloc.h"
gcc your_program.c -L/path/to/lib -lft_malloc -o your_program
```

Linking at runtime:

```bash
LD_LIBRARY=libft_malloc.so LD_LIBRARY_PATH=/path/to/lib ./your_program
```

Example using mallopt ENV variables:

```bash
MALLOC_CHECK_=3 MALLOC_PERTURB_=0 MALLOC_ZERO=0 MALLOC_SHOW_INFO_=1 ./your_program
```
