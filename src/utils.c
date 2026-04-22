#include "../include/malloc.h"


void	set_large_size(t_chunk *chunk, size_t zone_size) {

	chunk->prev_size = (u_int32_t)((zone_size >> 16) << 16);
    chunk->size = (u_int32_t)(((zone_size - sizeof(t_chunk)) << 16) >> 16);
	chunk->next = NULL;
}

size_t	get_large_size(t_chunk *chunk) {

	return ((size_t)(chunk->size + (size_t)get_prev_size(chunk)));
}

uint32_t		get_flags(t_chunk *chunk) {

	return (chunk->prev_size & ~FLAG_MASK);
}

uint32_t		get_prev_size(t_chunk *chunk) {

	return (chunk->prev_size & FLAG_MASK);
}

uint32_t	prev_in_use(t_chunk *chunk) {

	return (chunk->prev_size & PREV_INUSE);
}

void        set_prev_in_use(t_chunk *chunk) {

    chunk->prev_size |= PREV_INUSE;
}

void        unset_prev_in_use(t_chunk *chunk) {

    chunk->prev_size &= ~PREV_INUSE;
}

uint32_t		in_use(t_chunk *chunk) {

	return (chunk->prev_size & NEXT_INUSE);
}

void        set_in_use(t_chunk *chunk) {

    chunk->prev_size |= NEXT_INUSE;
}

void        unset_in_use(t_chunk *chunk) {

    chunk->prev_size &= ~NEXT_INUSE;
}

size_t		get_min(size_t a, size_t b) {

	return (((a <= b) ? a : b));
}

