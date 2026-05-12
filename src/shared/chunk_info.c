#include "../../include/malloc.h"


bool	has_flags(t_chunk *chunk, size_t flag) {

	if (!chunk)
		return (false);
	return (chunk->size & L_FLAG_MASK & flag) != 0;
}

void	set_flags(t_chunk *chunk, size_t flag) {

	if (!chunk)
		return ;
	chunk->size |= (flag & L_FLAG_MASK);
}

void	unset_flags(t_chunk *chunk, size_t flag) {

	if (!chunk)
		return ;
	chunk->size &= ~(flag & L_FLAG_MASK);
}

size_t	get_prevsize(t_chunk *chunk) {

	if (!chunk)
		return (0);
	return (chunk->prev_size & L_SIZE_MASK);
}

void	set_prevsize(t_chunk *chunk, size_t size) {

	if (!chunk)
		return ;
	size_t flags = chunk->prev_size & L_FLAG_MASK;
	chunk->prev_size = (size & L_SIZE_MASK) | flags;
}

size_t	get_size(t_chunk *chunk) {

	if (!chunk)
		return (0);
	return (chunk->size & L_SIZE_MASK);
}

void	set_size(t_chunk *chunk, size_t size) {

	if (!chunk)
		return ;
	size_t flags = chunk->size & L_FLAG_MASK;
	chunk->size = (size & L_SIZE_MASK) | flags;
}


size_t get_min(size_t a, size_t b) {

	return (((a <= b) ? a : b));
}

size_t to_decimal(char *addr) {

	return ((size_t)((addr)));
}