#ifndef MALLOC_H
#   define MALLOC_H

// MAP_ANON & MAP_ANONYMOUS FLAGS
#define _GNU_SOURCE



 // mmap(2)
#include <sys/mman.h>

 // munmap(2)
#include <sys/time.h>

 //getrlimit(2)
#include <sys/resource.h>

 // bool type
#include <stdbool.h>

 // uint32
#include <stdint.h>

 // abort() 
#include <stdlib.h>

 // thread functions
#include <pthread.h>

 // printf delete after
#include <stdio.h> 

 // self defined PREPROC
#include "./malloc_constants.h"

 // ft_put_ul, ft_put_hex, ft_memmove
#include "../libft/includes/libft.h"

 // structs for arena, chunk, heap, enums
#include "./malloc_types.h"


extern t_arena			g_arena;
extern pthread_mutex_t	g_lock;


void		arena_try_mutex_destroy_unlock(void);
void		arena_try_mutex_init_lock(void);
bool		arena_heap_uninitialized_or_large(size_t size);
void		arena_fastbin_unlink(t_chunk *chunk); 
t_chunk		*arena_fastbin_get(size_t size);
void		arena_fastbin_set(t_heap *heap, t_chunk *freed_chunk);
void		arena_fastbin_drain(t_heap *heap); 
int			arena_heap_munmap(t_heap *prev, t_heap *cur, t_heap **head);
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
void		 ft_free(void *ptr);
void    	*ft_realloc(void *ptr, size_t size);
int		free_internal(void *ptr);
void		*malloc_internal(size_t size);
void		*realloc_internal(void *ptr, size_t size);
void		show_alloc_mem(void);
void		show_alloc_mem_internal(void);
void		show_alloc_mem_ex(void);
void		show_alloc_mem_ex_internal(void);


//DELETE AFTER-----------------------------------------------------------------------


void printchunk(t_chunk *chunk);
void printheap(t_heap *heap, t_chunk *chunk);

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

//DELETE AFTER-----------------------------------------------------------------------

#endif
