#ifndef MALLOC_H
#   define MALLOC_H

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

 // max bytes for a tiny request
#define TINY_MAX 128

 // max bytes for a small request
#define SMALL_MAX 1024

 // 16384 - fits 128 tiny allocs
#define TINY_BLOCK_SIZE (4 * PAGE_SIZE)

 // 131072 - fits 128 small allocs
#define SMALL_BLOCK_SIZE (32 * PAGE_SIZE)

 // BLOCK ALLIGNMENT MULTIPLES OF 16
#define ALIGNMENT 16

size_t align(size_t size);

 // Heap group size
typedef enum s_head_group { TINY, SMALL, LARGE } t_head_group;


//  Metadata for a whole mmap'd region
typedef struct s_heap {

    struct s_heap   *prev;
    struct s_heap   *next;
    t_head_group    group;
    size_t          total_size;
    size_t          free_size;
    size_t          block_count;

}               t_heap;

// HEAP START GLOBAL VAR
extern t_heap *heap_start;

// HEAP FUNCTIONS
t_heap          *heap_find(size_t size);
t_head_group    heap_group(size_t size);
size_t          heap_size(size_t size);
t_heap          *heap_next(void);
t_heap          *heap_prev(void);
t_heap          *heap_new(size_t size);



// Metadata for a single allocated block
typedef struct s_block {

    struct s_block  *prev;
    struct s_block  *next;
    size_t          data_size;
    bool            available;

}               t_block;


 // BLOCK FUNCTIONS
t_block *block_find(t_heap *heap, size_t size);
void    block_merge(t_block *block, t_heap *heap);
void	block_split(t_block *block, size_t size);
void    *block_shift(t_block *block_addr);
void    *block_to_data(t_block *block);
t_block *heap_to_block(t_heap *heap);




// void ft_free(void *ptr);
// void *ft_malloc(size_t size);
// void *ft_realloc(void *ptr, size_t size);
// void    show_alloc_mem(void);

#endif
