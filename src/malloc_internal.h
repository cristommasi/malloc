// MAP_ANON & MAP_ANONYMOUS FLAGS
#ifndef _GNU_SOURCE
#   define _GNU_SOURCE
#endif

#ifndef MALLOC_INTERNAL_H
#   define MALLOC_INTERNAL_H

//---------------------------------------------------------------------------------------------INIT

// public header
#include "../malloc.h"

#define CONSTRUCTOR __attribute__((constructor))
#define DESTRUCTOR  __attribute__((destructor))

 // size_t type
#include <stddef.h>
#include <stdint.h>
 // uint(bits)_t types
#include <stdint.h>
 // bool type
#include <stdbool.h>

 // thread functions
#include <pthread.h>

 // used for err handling
extern void	abort(void) __attribute__((__noreturn__));
 // used in mallopts()
extern char *getenv(const char *name);

#if SIZE_MAX != 0xFFFFFFFFFFFFFFFFULL
#	error "size_t must be 8 bytes (64-bit platform required)"
#endif

 // BLOCK ALIGNMENT MULTIPLES OF 16
#define ALIGNMENT (2 * sizeof(size_t))

 // MACRO FN TO ALIGN
static inline size_t	ALIGN(size_t size) {

	return ( ( (size) + ALIGNMENT - 1 ) & ~( ALIGNMENT - 1 ) );
}

 // fw declarations
typedef struct s_heap t_heap;
typedef struct s_chunk t_chunk;
typedef enum e_heap_type t_heap_type;

 // internal functions (actual implementation)
void			*malloc_internal(size_t size);
int				free_internal(t_chunk *chunk);
void    		*realloc_internal(void *ptr, t_chunk *chunk, size_t old_size, size_t size);
void			show_alloc_mem_internal(void);
void			show_alloc_mem_ex_internal(int show_type);
int		        mallopt_internal(int param, int value);

//---------------------------------------------------------------------------------------------INIT
//---------------------------------------------------------------------------------------------ARENA

typedef struct MALLOC_OPS
{
	uint8_t			SHOW_INFO;
	uint8_t			PERTURB;
	uint8_t			CHECK;
	uint8_t			ZERO;

} MALLOC_OPS;


#define FASTBIN_COUNT 13
#define SMALLBIN_COUNT 116 

typedef struct s_arena {

	pthread_mutex_t		lock;
	MALLOC_OPS			OPS;
	
	t_heap  			*tiny;
	t_heap  			*small;
	t_heap  			*large;
	t_chunk 			*fastbin[FASTBIN_COUNT];
	t_chunk 			*smallbin[SMALLBIN_COUNT];
	t_heap				*tiny_cache;
	t_heap				*small_cache;

}               t_arena;

 // global var
extern t_arena			g_arena; 


t_heap		*arena_find_cached_heap(size_t zone_size);
void		arena_fastbin_unlink(t_chunk *chunk); 
t_chunk		*arena_fastbin_get(size_t size);
void		arena_fastbin_set(t_heap *heap, t_chunk *freed_chunk);
void		arena_fastbin_drain(t_heap *heap); 
t_chunk		*arena_smallbin_get(size_t size);
void        arena_smallbin_set(t_heap *heap, t_chunk *freed_chunk);
void        arena_smallbin_unlink(t_chunk *chunk);
void		arena_smallbin_drain(t_heap *heap);
int			arena_heap_munmap(t_heap *cur);
int			arena_heap_cache_or_munmap(t_heap *to_free, t_heap_type type);
void		arena_bin_set(t_heap *heap, t_chunk *chunk, t_heap_type type);
t_heap		*arena_heap_find_by_chunk(t_chunk *chunk);
t_heap		**arena_heap_group_by_chunk(size_t size); 
void		*arena_get_new_chunk_type(void *ptr, size_t p_new_size, size_t cur_size);
void		arena_heap_unlink(t_heap *heap, t_heap **head);
void    	arena_error_exit(int err);


 // MIN size to leave a chunk with 16 header + 16 data
#define MIN_TRIM 32

#define FASTBIN_MIN_CHUNK 16  
#define FASTBIN_MAX_CHUNK 208

#define SMALLBIN_MIN_CHUNK 224  
#define SMALLBIN_MAX_CHUNK 2064

 // get index fastbin
static inline int	FBIN_IDX(size_t data_size) {

	if (data_size >= FASTBIN_MIN_CHUNK && data_size <= FASTBIN_MAX_CHUNK) {

		return ( ((int)data_size - FASTBIN_MIN_CHUNK) / ALIGNMENT );
	}
	return (-1);
}

 // get index smallbin
static inline int	SBIN_IDX(size_t data_size) { 

	if (data_size >= SMALLBIN_MIN_CHUNK && data_size <=  SMALLBIN_MAX_CHUNK) {

		return ( ((int)data_size - SMALLBIN_MIN_CHUNK) / ALIGNMENT );
	}
	return (-1);
}


 // mallopt()
#define M_PARAM_ERROR                   0
#define M_PARAM_SUCCESS                 1
#define F_MUMMAP_ERROR                 -1
#define F_NO_ERROR                      0
#define F_INV_PTR_ERROR                 1
#define F_DOUBLE_FREE_ERROR             2
#define R_INV_PTR_ERROR                 3

 // error messages for free and realloc()
#define M_ERR_MSG_SIZE			 38
#define M_PARAM_ERR_MSG			 "free(): parameter out of bounds      \n"
#define M_HEAP_MAX_EXCEEDED_MSG  "free(): max number of arenas exceeded\n"
#define F_DOUBLE_FREE_MSG		 "free(): double free detected         \n"
#define F_INV_PTR_MSG			 "free(): invalid pointer              \n"
#define R_INV_PTR_MSG			 "realloc(): invalid pointer           \n"
#define F_MUNMAP_MSG			 "free(): munmap failed!               \n"
#define M_UNKNOWN_MSG			 "Unknown error                        \n"


static inline uint8_t	get_show_info(void) {
    
    return (g_arena.OPS.SHOW_INFO);
}

static inline bool		has_perturb(void) {

    return ((bool)g_arena.OPS.PERTURB);
}

static inline uint8_t	get_perturb_alloc(void) {

    return ((uint8_t)g_arena.OPS.PERTURB);
}

static inline uint8_t	get_perturb_free(void) {

    return (~((uint8_t)g_arena.OPS.PERTURB));
}

static inline uint8_t	get_check(void) {
    
    return (g_arena.OPS.CHECK);
}

static inline bool		has_check(void) {

    return ((bool)g_arena.OPS.CHECK);
}

static inline bool		has_zero(void) {

    return ((bool)g_arena.OPS.ZERO);
}

//---------------------------------------------------------------------------------------------ARENA
//---------------------------------------------------------------------------------------------HEAP

typedef enum e_heap_type { HEAP_TINY, HEAP_SMALL, HEAP_LARGE }	t_heap_type;

// sizeof(t_heap) 64 (40 + paddding)
typedef struct s_heap {

	size_t        	padding1, padding2, padding3;
	
	size_t        	alloc_chunks;
	size_t          total_size;
	t_chunk         *free_cis_start;
	struct s_heap   *next;
	struct s_heap   *prev;

}               t_heap;

size_t		heap_free_size(t_heap *heap);
t_heap		*heap_new(size_t zone_size); 
void		heap_append(t_heap **HEAP_TYPE, t_heap *new_heap);
t_chunk		*heap_find_cis_mem_chunk(size_t size) ;
t_chunk		*heap_split_cis_mem(t_heap *heap, size_t size);
size_t		heap_page_size(size_t size);
t_heap_type heap_type(size_t size);
bool		heap_is_different_type(size_t sizeA, size_t sizeB);
void		heap_update_alloc_chunks(t_heap *heap, int block);

static inline t_chunk		*heap_to_chunk(t_heap *heap) {

	uintptr_t	addr = (uintptr_t)heap + sizeof(t_heap);

	return ((t_chunk *)addr);
}

#define HEAP_TYPE_COUNT 3

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
#define TINY_HEAP_SIZE (8 * PAGE_SIZE)

 // 131072 - fits 128 small allocs
#define SMALL_HEAP_SIZE (64 * PAGE_SIZE)

static inline size_t	ALIGN_PAGE(size_t size) {

	return ( ( (size) + PAGE_SIZE - 1 ) & ~( PAGE_SIZE - 1 ) );
}

//---------------------------------------------------------------------------------------------HEAP
//---------------------------------------------------------------------------------------------CHUNK

typedef struct s_chunk {

	size_t			prev_size;
	size_t			size;
	struct s_chunk	*next; //only used if free otherwise used as client data
	struct s_chunk	*prev; //only used if free otherwise used as client data

}                   t_chunk;

t_chunk		*chunk_split_center(t_heap *heap, t_chunk *chunk, size_t old_size, size_t new_size);
t_chunk		*chunk_split_right(t_heap *heap, t_chunk *chunk, t_chunk *next, size_t need);
t_chunk		*chunk_split_left(t_heap *heap, t_chunk *chunk, t_chunk *prev, size_t need);
t_chunk		*chunk_split_cis(t_heap *heap, t_chunk *chunk, size_t need, size_t size);
t_chunk		*chunk_realloc_in_place(t_heap *heap, t_chunk *chunk, size_t size);
bool		chunk_belongs_to_heap(t_heap *heap, t_chunk *chunk);
t_chunk		*chunk_next(t_heap *heap, t_chunk *chunk);
t_chunk		*chunk_prev(t_heap *heap, t_chunk *chunk);
bool		chunk_prev_suffices(t_chunk *prev, size_t need);
bool		chunk_next_suffices(t_chunk *next, size_t need);
t_chunk		*chunk_new(char *start, size_t prev_s, size_t size, size_t flags);
t_chunk		*chunk_coalesce(t_heap *heap, t_chunk *freed_chunk);
bool	    chunk_already_freed(t_chunk *chunk);
bool	    chunk_is_cis_mem(t_chunk *chunk);


#define TINY_CHUNK_MIN		16  // min bytes for a tiny request
#define TINY_CHUNK_MAX		208  // max bytes for a tiny request
#define SMALL_CHUNK_MIN		224  // min bytes for a small request
#define SMALL_CHUNK_MAX		2064  // max bytes for a small request
#define LARGE_CHUNK_MIN		2065  // min default bytes for large request
#define CHUNK_INUSE_SIZE	(size_t)16  // size of chunk header when in use (prev_size + size)
#define CHUNK_FREE_SIZE		(size_t)32  // size of chunk header when free (prev_size + size + next + prev)

static inline void		*chunk_to_data(t_chunk *chunk_addr) {

	return ( (void *)((char*)chunk_addr + CHUNK_INUSE_SIZE) );
}

static inline t_chunk    *data_to_chunk(void *data_addr) {

	return ( (t_chunk *)((char*)data_addr - CHUNK_INUSE_SIZE) );
}

 // masks for chunk->size in LSBs
#define NO_FLAGS     ((size_t)0)
#define IN_USE       ((size_t)1)
#define IS_CIS       ((size_t)2)
#define FLAG_MASK	 ((size_t)3)
#define SIZE_MASK    (~(size_t)3)

static inline bool		has_flags(t_chunk *chunk, size_t flag) {

	if (chunk) {
		return (chunk->size & FLAG_MASK & flag) != 0;
	}
	return (false);
}

static inline void		set_flags(t_chunk *chunk, size_t flag) {

	if (chunk) {
		chunk->size |= (flag & FLAG_MASK);
	}
}

static inline void		unset_flags(t_chunk *chunk, size_t flag) {

	if (chunk) {
		chunk->size &= ~(flag & FLAG_MASK);
	}
}

static inline size_t    get_prevsize(t_chunk *chunk) {

	if (chunk) {
		return (chunk->prev_size);
	}
	return (0);
}

static inline void		set_prevsize(t_chunk *chunk, size_t size) {

	if (chunk) {
		chunk->prev_size = (size & SIZE_MASK);
	}
}

static inline size_t	get_size(t_chunk *chunk) {

	if (chunk) {
		return (chunk->size & SIZE_MASK);
	}
	return (0);
}

static inline void      set_size(t_chunk *chunk, size_t size) {

	if (chunk) {
		size_t flags = chunk->size & FLAG_MASK;
		chunk->size = (size & SIZE_MASK) | flags;
	}
}

//---------------------------------------------------------------------------------------------CHUNK
//---------------------------------------------------------------------------------------------UTILS

void		*move_data(void *dest, const void *src, size_t n);
void		*do_perturb(void *s, unsigned int c, size_t n);
int     	asciitoint(const char *str);
void    	print_hex_byte(unsigned char byte, int mode);
void    	print_hex_addr(uintptr_t n, int mode);
void		print_long(unsigned long n);
void		print_string(char *s);
size_t      print_data_in_chunk(char *cur_chunk, size_t chunk_size, int offset);
void    	print_data_bytes(char *data, size_t len);
void        print_heap_type(int index, t_heap *cur);
void        print_chunk_addr(t_chunk *cur_chunk, size_t chunk_size);


#define HEX_DUMP_HEADER_TXT "Address             Hex bytes                                ASCII\n"
#define HEX_LOWER_CASE 0
#define HEX_UPPER_CASE 1

//---------------------------------------------------------------------------------------------UTILS

#endif
