#include "../include/malloc.h"

/*
typedef struct s_chunk {

    size_t          size;
    struct s_chunk  *prev;
    struct s_chunk  *next;
}                   t_chunk;
*/


void    *chunk_to_data(t_chunk *block_addr) {

	return ((void *)(block_addr + 1));
}




