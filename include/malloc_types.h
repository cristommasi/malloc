#ifndef MALLOC_TYPES_H
#   define MALLOC_TYPES_H

 // abort() - size_t
#include <stdlib.h>

 // uint(bits)_t types
#include <stdint.h>


typedef struct s_chunk {

	size_t			prev_size;
	size_t			size;
	struct s_chunk	*next;
	struct s_chunk	*prev;
}                   t_chunk;

// can be 64
typedef struct s_heap {

	size_t        	blocks;
	size_t          total_size;
	t_chunk         *free_cis_start;
	struct s_heap   *next;
	struct s_heap   *prev;

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
	t_chunk 			*fastbin[10];
	t_chunk 			*smallbin[56];
	uint32_t			count;

}               t_arena;

#endif
