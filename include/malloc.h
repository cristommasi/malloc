#ifndef MALLOC_H
#   define MALLOC_H
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
// MAP_ANON & MAP_ANONYMOUS FLAGS
#define _GNU_SOURCE
 // mmap(2)
#include <sys/mman.h>
 // munmap(2)
#include <sys/time.h>
 // getpagesize / sysconf(_SC_PAGESIZE) / write()
#include <unistd.h>
 //getrlimit(2)
#include <sys/resource.h>
 // thread functions
#include <pthread.h>
 // libft
#include "./libft/includes/libft.h"
 // bool type
#include <stdbool.h>
 // printf
#include <stdio.h> 


 // SYSTEM DEFAULT PAGE_SIZE (4096)
#ifdef __APPLE__
#   define PAGE_SIZE (size_t)getpagesize()
#else
#   define PAGE_SIZE (size_t)sysconf(_SC_PAGESIZE)
#endif

 // SYSTEM DEFAULT PROT FLAGS
#define PROT_FLAGS (PROT_READ | PROT_WRITE)


 // SYSTEM DEFAULT MAP FLAGS
#ifdef __APPLE__ 
#   define MAP_FLAGS (MAP_PRIVATE | MAP_ANON)
#else
#   define MAP_FLAGS (MAP_PRIVATE | MAP_ANONYMOUS)
#endif

// NO FD FLAG
#define NO_FD -1

// NO OFFSET FLAG
#define NO_OFFSET 0

 // BLOCK ALLIGNMENT MULTIPLES OF 8
#define ALIGNMENT (sizeof(size_t))

 // MACRO FN TO ALIGN
#define ALIGN(size) (((size) + ALIGNMENT - 1) & ~(ALIGNMENT - 1))

 // max bytes for a tiny request
#define TINY_CHUNK_MAX 128

 // max bytes for a small request
#define SMALL_CHUNK_MAX 1024

 // 16384 - fits 128 tiny allocs
#define TINY_HEAP_SIZE (4 * PAGE_SIZE)

 // 131072 - fits 128 small allocs
#define SMALL_HEAP_SIZE (32 * PAGE_SIZE)

 // 16, 32, 48, 64, 80, 96, 112, 128
#define FASTBIN_MIN_CHUNK 16

 // 0, 1, 2, 3, 4, 5, 6, 7
#define FASTBIN_COUNT 64

#define BIN_IDX(size) (((size - FASTBIN_MIN_CHUNK) / ALIGNMENT))

#define BIN_LAST 0

typedef enum {TINY_HEAP, SMALL_HEAP, LARGE_HEAP} t_heap_type;



// Metadata for a single allocated block
typedef struct s_chunk {

    size_t          size;
    struct s_chunk  *next;
}                   t_chunk;

//  Metadata for a whole mmap'd region
typedef struct s_heap {

    size_t          total_size;
    size_t          blocks;
    struct s_heap   *next;
    t_chunk         *free_cis_start;

}               t_heap;


typedef struct s_arena {

    t_heap  *tiny;
    t_heap  *small;
    t_heap  *large;

    t_chunk *fastbin[FASTBIN_COUNT]; // array of s-linked-list of sizes 16, 32, 48, 64, 80, 96, 112, 128 (LIFO)
}               t_arena;


extern t_arena g_arena;


 
 // HEAP FUNCTIONS
t_heap      *heap_new_and_append(size_t size);
t_heap      *heap_new(size_t zone_size);
void        heap_append(t_heap **HEAP_TYPE, t_heap *new_heap);
t_heap      *heap_find_cis_mem(size_t size);
t_chunk		*heap_split_cis_mem(t_heap *heap, size_t size);
int         heap_has_remaining_cis(t_heap *heap, size_t size);
t_chunk     *heap_to_chunk(t_heap *heap_addr);
void        *chunk_to_data(t_chunk *chunk_addr);
t_chunk    *data_to_chunk(void *data_addr);
size_t      heap_page_size(size_t size);
t_heap_type heap_type(size_t size);
size_t      heap_cis_mem_size(t_heap *heap);
size_t      heap_free_size(t_heap *heap);
bool        chunk_covers_entire_heap(t_heap *heap, t_chunk *chunk);


 // ARENA FUNCTIONS
bool         arena_heap_uninitialized_or_large(size_t size);
t_chunk     *arena_fastbin_get(size_t size);
void        arena_fastbin_set(t_heap *heap, t_chunk *freed_chunk);
void        arena_fastbin_drain(t_heap *heap);
t_heap      **arena_heap_group(size_t size);
t_heap      *arena_heap_find_by_size(t_chunk *chunk, size_t size);
bool        arena_owner_of_heap(t_heap *heap, t_chunk *chunk);
int         arena_heap_munmap(t_heap *prev, t_heap *cur, t_heap **head);

void printall(t_heap *heap_type);void printALL(void);


void ft_free(void *ptr);
void *ft_malloc(size_t size);
// void *ft_realloc(void *ptr, size_t size);
// void    show_alloc_mem(void);

#endif

