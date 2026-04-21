#include "../include/malloc.h"


void	set_large_size(t_chunk *chunk, size_t zone_size) {

	chunk->prev_size = (u_int32_t)((zone_size >> 16) << 16);
    chunk->size = (u_int32_t)(((zone_size - sizeof(t_chunk)) << 16) >> 16);
	chunk->next = NULL;
}

size_t	get_large_size(t_chunk *chunk) {

	return ((size_t)(chunk->size + (size_t)get_prev_size(chunk->prev_size)));
}

uint32_t		get_prev_inuse(t_chunk *chunk) {

	return (chunk->prev_size & 0xFFFF);
}

void        set_prev_inuse(t_chunk *chunk) {

    chunk->prev_size = chunk->prev_size |= PREV_INUSE;
}

void        unset_prev_inuse(t_chunk *chunk) {

    chunk->prev_size = chunk->prev_size &= 0xFFFF;
}

bool	heap_is_large(size_t size) {

	size_t	zone_size = heap_page_size(size);

	return (zone_size != TINY_HEAP_SIZE && zone_size != SMALL_HEAP_SIZE);

}

