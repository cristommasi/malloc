#ifndef MALLOC_INTERNAL_H
#   define MALLOC_INTERNAL_H


// MAP_ANON & MAP_ANONYMOUS FLAGS
#define _GNU_SOURCE

#include "./malloc.h"

#define CONSTRUCTOR __attribute__((constructor))
#define DESTRUCTOR  __attribute__((destructor))

 // size_t
#include <stddef.h>
#include <stdint.h>

#if SIZE_MAX != 0xFFFFFFFFFFFFFFFFULL
#	error "size_t must be 8 bytes (64-bit platform required)"
#endif

 // BLOCK ALIGNMENT MULTIPLES OF 8
#define ALIGNMENT (2 * sizeof(size_t))

 // MACRO FN TO ALIGN
size_t		ALIGN(size_t size);

 // valgrind macros
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>

extern void abort(void) __attribute__((__noreturn__));

void			*malloc_internal(size_t size);
int				free_internal(void *ptr);
void			*realloc_internal(void *ptr, size_t size);
void			show_alloc_mem_internal(void);
void			show_alloc_mem_ex_internal(void);
int		        mallopt_internal(int param, int value);



 // uint(bits)_t types
#include <stdint.h>
 // bool
#include <stdbool.h>

extern char *getenv(const char *name);

typedef struct MALLOC_OPS
{
	uint8_t			SHOW_INFO; // free and alloc fill with bits
	uint8_t			PERTURB; // free and alloc fill with bits
	uint8_t			CHECK; // print msg or abort
	uint32_t		ARENA_MAX; //max amount of arenas

} MALLOC_OPS;

uint8_t     get_show_info(void);
uint8_t     get_check(void);
bool        has_perturb(void);
int         get_perturb_alloc(void);
int         get_perturb_free(void);
void		*do_perturb(void *s, int c, size_t n);
bool        has_check(void); 
bool        has_arena_max(void); 
uint32_t    get_arena_max(void);
int     	asciitoint(const char *str);

 // mallopt()
#define M_PARAM_ERROR                   0
#define M_PARAM_SUCCESS                 1
#define F_MUMMAP_ERROR                 -1
#define F_NO_ERROR                      0
#define F_INV_PTR_ERROR                 1
#define F_DOUBLE_FREE_ERROR             2

 // error messages for free and mallopt()
#define M_PARAM_ERR_MSG "mallopt(): parameter out of bounds      \n"
#define M_ARENA_MAX_EXCEEDED_MSG "malloc(): max number of arenas exceeded \n"
#define F_DOUBLE_FREE_MSG "free(): double free detected in tcache 2\n"
#define F_INV_PTR_MSG "free(): invalid pointer                 \n"
#define F_INV_PTR_MSG3 "fre3(): invalid pointer                 \n"
#define F_INV_PTR_MSG4 "fre4(): invalid pointer                 \n"
#define F_MUNMAP_MSG "free(): munmap failed!                  \n"
#define F_ABORT_MSG "abort()                                 \n"




 // fw declaration
typedef struct s_heap t_heap;
typedef struct s_chunk t_chunk;

typedef struct s_arena {

	MALLOC_OPS			OPS;
	
	t_heap  			*tiny;
	t_heap  			*small;
	t_heap  			*large;
	t_chunk 			*fastbin[7];
	t_chunk 			*smallbin[56];
	uint32_t			count;

}               t_arena;

 // thread functions
#include <pthread.h>
 // global lock
extern pthread_mutex_t	g_lock;
 // global var
extern t_arena			g_arena;

void		arena_fastbin_unlink(t_chunk *chunk); 
t_chunk		*arena_fastbin_get(size_t size);
void		arena_fastbin_set(t_heap *heap, t_chunk *freed_chunk);
void		arena_fastbin_drain(t_heap *heap); 
t_chunk		*arena_smallbin_get(size_t size);
void        arena_smallbin_set(t_heap *heap, t_chunk *freed_chunk);
void        arena_smallbin_unlink(t_chunk *chunk);
void		arena_smallbin_drain(t_heap *heap);
int			arena_heap_munmap(t_heap *cur, t_heap **head);
t_heap		*arena_heap_find_by_chunk(t_chunk *chunk);
t_heap		**arena_heap_group_by_chunk(size_t size); 
void		*arena_get_new_chunk_type(void *ptr, size_t p_new_size, size_t cur_size);
void		arena_heap_unlink(t_heap *heap, t_heap **head);

 // MIN size to leave a chunk with 16 header + 16 data
#define MIN_TRIM 32

 //  16, 32, 48, 64, 80, 96, 112
#define FASTBIN_MIN_CHUNK 16

 //  128, 144, 160, 176, ...
#define SMALLBIN_MIN_CHUNK 128

 // 0, 1, 2, 3, 4, 5, 6, 7
#define FASTBIN_COUNT 7

 // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, ...
#define SMALLBIN_COUNT 56

 // get index fastbin
int		FBIN_IDX(size_t size);

 // get index smallbin
int		SBIN_IDX(size_t size);



// sizeof(t_heap) 64 for alignment
typedef struct s_heap {

	size_t        	p1;
	size_t        	p2;
	size_t        	p3;
	size_t        	blocks;
	size_t          total_size;
	t_chunk         *free_cis_start;
	struct s_heap   *next;
	struct s_heap   *prev;

}               t_heap;

typedef enum e_heap_type { HEAP_TINY, HEAP_SMALL, HEAP_LARGE }	t_heap_type;

t_heap		*heap_new_and_append(size_t size);
size_t		heap_free_size(t_heap *heap);
t_heap		*heap_new(size_t zone_size); 
void		heap_append(t_heap **HEAP_TYPE, t_heap *new_heap);
t_chunk		*heap_find_cis_mem_chunk(size_t size) ;
t_chunk		*heap_split_cis_mem(t_heap *heap, size_t size);
t_chunk		*heap_to_chunk(t_heap *heap_addr);
size_t		heap_page_size(size_t size);
t_heap_type heap_type(size_t size);
bool		heap_is_different_type(size_t sizeA, size_t sizeB);

 //getrlimit(2)
#include <sys/resource.h>

#define USERSPACE_MAX ((size_t)0x7FFFFFFFFFFF)

int     size_exceeds_rlimit(size_t aligned_size);

 // getpagesize / sysconf(_SC_PAGESIZE) / write()
#include <unistd.h>

 // SYSTEM DEFAULT PAGE_SIZE (4096)
#ifdef __APPLE__
#   define PAGE_SIZE (size_t)getpagesize()
#else
#   define PAGE_SIZE (size_t)sysconf(_SC_PAGESIZE)
#endif

 // mmap(2) munmap(2)
#include <sys/mman.h>

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

 // 16384 - fits 128 tiny allocs
#define TINY_HEAP_SIZE (4 * PAGE_SIZE)

 // 131072 - fits 128 small allocs
#define SMALL_HEAP_SIZE (32 * PAGE_SIZE)





typedef struct s_chunk {

	size_t			prev_size;
	size_t			size;
	struct s_chunk	*next;
	struct s_chunk	*prev;
}                   t_chunk;

void		chunk_split_center(t_heap *heap, t_chunk *chunk, size_t need);
void		chunk_split_right(t_heap *heap, t_chunk *chunk, t_chunk *next, size_t need);
void		chunk_split_left(t_heap *heap, t_chunk *chunk, t_chunk *prev, size_t need);
t_chunk		*chunk_realloc_in_place(t_heap *heap, t_chunk *chunk, size_t size);
bool		chunk_belongs_to_heap(t_heap *heap, t_chunk *chunk);
t_chunk		*get_next_chunk(t_heap *heap, t_chunk *chunk);
t_chunk		*get_prev_chunk(t_heap *heap, t_chunk *chunk);
bool		prev_chunk_suffices(t_chunk *prev, size_t need);
bool		next_chunk_suffices(t_chunk *next, size_t need);
void		*chunk_to_data(t_chunk *chunk_addr);
void		chunk_relink(t_chunk *prev, t_chunk *center, t_chunk *next);
t_chunk		*data_to_chunk(void *data_addr);
t_chunk		*chunk_new(char *start, size_t prev_s, size_t size, size_t flags);
t_chunk		*chunk_coalesce(t_heap *heap, t_chunk *freed_chunk);

 // max bytes for a tiny request
#define TINY_CHUNK_MAX 112

 // max bytes for a small request
#define SMALL_CHUNK_MAX 1008

 // min default bytes for large request
#define LARGE_CHUNK_MIN 1009

 // size of chunk header when in use (prev_size + size)
#define CHUNK_INUSE_SIZE (size_t)16

 // size of chunk header when free (prev_size + size + next + prev)
#define CHUNK_FREE_SIZE (size_t)32




void		set_size(t_chunk *chunk, size_t size);
size_t		get_size(t_chunk *chunk);
void		set_prevsize(t_chunk *chunk, size_t size);
size_t		get_prevsize(t_chunk *chunk); 
void		set_flags(t_chunk *chunk, size_t flag);
void		unset_flags(t_chunk *chunk, size_t flag);
bool		has_flags(t_chunk *chunk, size_t flag);
void		*move_data(void *dest, const void *src, size_t n);
bool	    is_invalid_memory(t_chunk *chunk);
bool	    already_freed(t_chunk *chunk);

 // masks
#define NO_FLAGS     ((size_t)0)
#define IN_USE       ((size_t)1)
#define IS_CIS       ((size_t)2)
#define IS_LARGE     ((size_t)4)
#define L_FLAG_MASK  ((size_t)7)
#define L_SIZE_MASK  ((size_t)18446744073709551608UL)




bool		printable_char(int c);
void    	print_hex_byte(unsigned char byte, int mode);
void    	print_hex_addr(uintptr_t n, int mode);
void		print_long(unsigned long n);
void		print_string(char *s);
size_t      print_data_in_chunk(char *cur_chunk, size_t chunk_size, int offset);
void    	print_data_bytes(char *data, size_t len);
void        print_heap_type(int index, t_heap *cur);
void        print_chunk_addr(t_chunk *cur_chunk, size_t chunk_size);

#define HEAP_TYPE_COUNT 3
#define HEX_DUMP_HEADER_TXT "Address             Hex bytes                                ASCII\n"
#define HEX_LOWER_CASE 0
#define HEX_UPPER_CASE 1


#endif
