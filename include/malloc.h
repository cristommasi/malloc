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

 // libft
#include "./libft/includes/libft.h"
 // bool type
#include <stdbool.h>
 // uint32
#include <stdint.h>
 // abort() 
#include <stdlib.h>
 // thread functions
#include <pthread.h>

 // printf
#include <stdio.h> 


#if SIZE_MAX != 0xFFFFFFFFFFFFFFFFULL
#	error "size_t must be 8 bytes (64-bit platform required)"
#endif

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


#define F_MUMMAP_ERROR -1

#define F_NO_ERROR 0

#define F_DOUBLE_FREE_ERROR 1

#define F_INV_PTR_ERROR 2


#define F_DOUBLE_FREE_MSG "free(): double free detected in tcache 2\n"

#define F_INV_PTR_MSG "free(): invalid pointer\n"

#define F_MUNMAP_MSG "free(): munmap failed!\n"

#define F_ABORT_MSG "abort()\n"

// NO FD FLAG
#define NO_FD -1

// NO OFFSET FLAG
#define NO_OFFSET 0

 // BLOCK ALIGNMENT MULTIPLES OF 8
#define ALIGNMENT (sizeof(size_t))

 // MACRO FN TO ALIGN
#define ALIGN(size) (((size) + ALIGNMENT - 1) & ~(ALIGNMENT - 1))

#define TINY_SMALL_BLOCK_MAX 128

 // max bytes for a tiny request
#define TINY_CHUNK_MAX 112

 // max bytes for a small request
#define SMALL_CHUNK_MAX 1008

#define LARGE_CHUNK_MIN 1009

 // 16384 - fits 128 tiny allocs
#define TINY_HEAP_SIZE (4 * PAGE_SIZE)

 // 131072 - fits 128 small allocs
#define SMALL_HEAP_SIZE (32 * PAGE_SIZE)

 // 16, 32, 48, 64, 80, 96, 112, 128
#define FASTBIN_MIN_CHUNK 16

 // 0, 1, 2, 3, 4, 5, 6, 7
#define FASTBIN_COUNT 64

#define MIN_TRIM 32

// index 0 doesnt exist
#define BIN_IDX(size) (((size - FASTBIN_MIN_CHUNK) / ALIGNMENT))

#define BIN_LAST 0

 // masks
#define IN_USE        0b001
#define IS_CIS        0b010
#define IS_LARGE      0b100
#define TS_FLAG_MASK  0b00000000000000000000000000000111U
#define TS_SIZE_MASK  0b11111111111111111111111111111000U 
#define L_FLAG_MASK   ((size_t)0x7)
#define L_SIZE_MASK   (~(size_t)0x7)


typedef struct t_small_tiny
{
	uint32_t	size;
    uint16_t	next_size;
	uint16_t	prev_size;

}	t_small_tiny;


typedef struct s_large {
	
	size_t size;
	
}	t_large;



typedef struct s_chunk {

	union {
		t_small_tiny	small;
		t_large		    large;
	};
	struct s_chunk	*next;
}                   t_chunk;


typedef struct s_heap {

	uint32_t        blocks;
	size_t          total_size;
	struct s_heap   *next;
	t_chunk         *free_cis_start;

}               t_heap;

typedef enum s_init {UNINITIALIZED, INITIALIZED} t_init;

typedef struct s_arena {

	t_heap  *tiny;
	t_heap  *small;
	t_heap  *large;

	t_init			state;
	pthread_mutex_t	lock;
	t_chunk *fastbin[FASTBIN_COUNT]; // array of s-linked-list of sizes 16, 32, 48, 64, 80, 96, 112, 128 (LIFO)
}               t_arena;


extern t_arena g_arena;



bool		arena_heap_uninitialized_or_large(size_t size);
void		arena_fastbin_unlink(t_chunk *chunk); 
t_chunk		*arena_fastbin_get(size_t size);
void		arena_fastbin_set(t_heap *heap, t_chunk *freed_chunk);
void		arena_fastbin_drain(t_heap *heap); 
int		arena_heap_munmap(t_heap *prev, t_heap *cur, t_heap **head);
t_heap		*arena_heap_find_by_chunk(t_chunk *chunk);
t_heap		**arena_heap_group(size_t size); 
void		*arena_get_new_chunk(void *ptr, size_t p_new_size, size_t cur_size);


t_heap		*heap_new_and_append(size_t size);
size_t		heap_free_size(t_heap *heap);
t_heap		*heap_new(size_t zone_size); 
void		heap_append(t_heap **HEAP_TYPE, t_heap *new_heap);
t_chunk		*heap_find_cis_mem_chunk(size_t size) ;
t_chunk		*heap_split_cis_mem(t_heap *heap, size_t size);
bool		heap_has_remaining_cis(t_heap *heap, size_t size); 
size_t		heap_cis_mem_size(t_heap *heap);
bool		heap_cis_mem_fits_chunk(t_heap *heap, size_t to_add);
t_chunk		*heap_to_chunk(t_heap *heap_addr);
size_t		heap_page_size(size_t size);
size_t		heap_chunk_size(size_t size);
size_t		heap_type(size_t size);
bool		heap_is_different_type(size_t sizeA, size_t sizeB);


void		chunk_split_center(t_heap *heap, t_chunk *chunk, size_t need);
void		chunk_split_right(t_heap *heap, t_chunk *chunk, t_chunk *next, size_t need);
void		chunk_split_left(t_heap *heap, t_chunk *chunk, t_chunk *prev, size_t need);
t_chunk		*heap_realloc_in_place(t_heap *heap, t_chunk *chunk, size_t size);
bool		chunk_covers_entire_heap(t_heap *heap, t_chunk *chunk);
bool		chunk_belongs_to_heap(t_heap *heap, t_chunk *chunk);
t_chunk		*get_next_chunk(t_heap *heap, t_chunk *chunk);
t_chunk		*get_prev_chunk(t_heap *heap, t_chunk *chunk);
bool		prev_chunk_suffices(t_chunk *prev, size_t need);
bool		next_chunk_suffices(t_chunk *next, size_t need);
void		*chunk_to_data(t_chunk *chunk_addr);
void		chunk_relink(t_chunk *prev, t_chunk *center, t_chunk *next);
t_chunk		*data_to_chunk(void *data_addr);


void		set_size(t_chunk *chunk, size_t size);
size_t		get_size(t_chunk *chunk);
void		set_prevsize(t_chunk *chunk, size_t size);
size_t		get_prevsize(t_chunk *chunk); 
void		set_nextsize(t_chunk *chunk, size_t size);
size_t		get_nextsize(t_chunk *chunk);
void		set_flags(t_chunk *chunk, size_t flag);
void		unset_flags(t_chunk *chunk, size_t flag);
bool		has_flags(t_chunk *chunk, size_t flag);
bool		is_large(t_chunk *chunk);
size_t		get_min(size_t a, size_t b);
void		print_heap_type(int index, t_heap *cur);
void		print_chunk(char *start, char *end, size_t bytes);


void		*ft_malloc(size_t size);
int		     ft_free(void *ptr);
void    	*ft_realloc(void *ptr, size_t size);
void		free_internal(void *ptr);
void		*malloc_internal(size_t size);
void		*realloc_internal(void *ptr, size_t size);
void		show_alloc_mem(void);


//DELETE AFTER

void printchunk(t_chunk *chunk);
void printheap(t_heap *heap, t_chunk *chunk);

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

#endif
