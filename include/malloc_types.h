#ifndef MALLOC_TYPES_H
#   define MALLOC_TYPES_H

 // abort() - size_t
#include <stdlib.h>

 // uint(bits)_t types
#include <stdint.h>


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
	struct s_chunk	*prev;
}                   t_chunk;


typedef struct s_heap {

	uint32_t        blocks;
	size_t          total_size;
	struct s_heap   *next;
	struct s_heap   *prev;
	t_chunk         *free_cis_start;

}               t_heap;


typedef struct MALLOC_OPS
{
	uint8_t			PERTURB; // free and alloc fill with bits
	uint8_t			CHECK; // print msg or abort
	uint32_t		ARENA_MAX; //max amount of arenas
	size_t			MMAP_THRESHOLD; // min before calling mmap()

} MALLOC_OPS;


typedef struct s_arena {

	MALLOC_OPS			OPS;

	
	t_heap  			*tiny;
	t_heap  			*small;
	t_heap  			*large;
	t_chunk 			*fastbin[64];
	uint32_t			count;

}               t_arena;

#endif
