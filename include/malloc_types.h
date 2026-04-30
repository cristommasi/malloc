#ifndef MALLOC_TYPES_H
#   define MALLOC_TYPES_H

 // abort() - size_t
#include <stdlib.h>

 // uint(bits)_t types
#include <stdint.h>

typedef enum s_init {

	UNINITIALIZED = 0, 
	INITIALIZED = 1

} t_init;


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



typedef struct MALLOC_ENV_VARS
{
	unsigned int	MALLOC_CHECK; // print msg or abort
	char			MALLOC_PERTURB; // free and alloc fill with bits
	unsigned int	MALLOC_ARENA_MAX; //max amount of arenas
	size_t			MALLOC_MMAP_THRESHOLD; // min before calling mmap()

} MALLOC_ENV_VARS;


typedef struct s_arena {

	t_init  			state;
	MALLOC_ENV_VARS		env;

	t_heap  			*tiny;
	t_heap  			*small;
	t_heap  			*large;
	t_chunk 			*fastbin[64];

	
}               t_arena;

#endif
