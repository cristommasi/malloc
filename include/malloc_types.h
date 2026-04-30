#ifndef MALLOC_TYPES_H
#   define MALLOC_TYPES_H

#include <stdlib.h>
#include <stdint.h>

typedef enum s_init {

	UNINITIALIZED = 0, 
	INITIALIZED = 1

} t_init;

typedef enum s_MALLOC_CHECK {
	
	SILENT = 0, 
	PRINT = 1, 
	ABORT = 2, 
	PRINT_ABORT = 3

} t_MALLOC_CHECK;

typedef enum s_MALLOC_SCRIBBLE {

	SCRIBBLE_NONE = 0,
	SCRIBBLE_DEFAULT = 1

} t_MALLOC_SCRIBBLE;


typedef enum s_MALLOC_PERTURB {

	PERTURB_NONE = 0,
	PERTURB_CUSTOM = 1
	
} t_MALLOC_PERTURB;


typedef enum s_MALLOC_VERBOSE {

	VERBOSE_NONE = 0,
	VERBOSE_YES = 1
	
} t_MALLOC_VERBOSE;

typedef enum s_MALLOC_HIST_SIZE {

	HISTORY_NONE = 0,
	HISTORY_KEEP = 1
	
} t_MALLOC_HIST_SIZE;

typedef enum s_MALLOC_MMAP_THRESHOLD {

	DEFAULT = 0,
	CUSTOM = 1
	
} t_MALLOC_MMAP_THRESHOLD;


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


typedef struct s_arena {

	t_init  				state;


	t_MALLOC_CHECK			check;
	t_MALLOC_SCRIBBLE 		scribble;
	t_MALLOC_PERTURB		perturb;
	unsigned char			perturb_byte;
	t_MALLOC_VERBOSE		verbose;
	t_MALLOC_HIST_SIZE		history;
	t_MALLOC_MMAP_THRESHOLD	min_large;
	size_t					min_large_size;

	t_heap  			*tiny;
	t_heap  			*small;
	t_heap  			*large;
	t_chunk 			*fastbin[64];

	
}               t_arena;

#endif