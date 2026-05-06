#include "../../include/malloc.h"

bool is_large(t_chunk *chunk) {

	if (!chunk)
		return false;
	return (chunk->large.size & IS_LARGE) != 0;
}

bool has_flags(t_chunk *chunk, size_t flag) {

	if (!chunk)
		return false;

	return (chunk->small.size & TS_FLAG_MASK & (uint32_t)flag) != 0;
}


void set_flags(t_chunk *chunk, size_t flag) {

	if (!chunk)
		return;

	chunk->small.size |= (uint32_t)(flag & TS_FLAG_MASK);
}


void unset_flags(t_chunk *chunk, size_t flag) {

	if (!chunk)
		return;

	chunk->small.size &= ~(uint32_t)(flag & TS_FLAG_MASK);
}


size_t get_prevsize(t_chunk *chunk) {

	if (!chunk || is_large(chunk))
		return (0);
	return (size_t)chunk->small.prev_size;
}


void set_prevsize(t_chunk *chunk, size_t size) {

	if (!chunk || is_large(chunk))
		return ;
	chunk->small.prev_size = (uint16_t)size;
}


size_t get_size(t_chunk *chunk) {

	if (!chunk)
		return 0;

	if (is_large(chunk))
		return chunk->large.size & L_SIZE_MASK;

	return (size_t)(chunk->small.size & TS_SIZE_MASK);
}

void set_size(t_chunk *chunk, size_t size)
{
	if (!chunk) return;

	if (is_large(chunk))
	{
		size_t flags = chunk->large.size & L_FLAG_MASK;
		chunk->large.size = (size & L_SIZE_MASK) | flags;
		return;
	}
	uint32_t flags = chunk->small.size & TS_FLAG_MASK;
	chunk->small.size = (uint32_t)(size & TS_SIZE_MASK) | flags;
}


void set_nextsize(t_chunk *chunk, size_t size) {

	if (!chunk || is_large(chunk))
		return ;
	chunk->small.next_size = (uint16_t)size;
}

size_t get_nextsize(t_chunk *chunk) {

	if (!chunk || is_large(chunk))
		return (0);
	return (size_t)chunk->small.next_size;
}

bool	heap_end(t_heap *heap, void *addr) {

	return ((char*)heap + sizeof(t_heap) + heap->total_size == (char*)addr);
}

size_t heap_free_size(t_heap *heap) {

	if (!heap || !heap->free_cis_start)
		return (0);
	char *end = (char*)heap + sizeof(t_heap) + heap->total_size;

	return ((size_t)(end - (char*)heap->free_cis_start));
}

size_t		get_min(size_t a, size_t b) {

	return (((a <= b) ? a : b));
}

size_t  to_decimal(char *addr) {

	return ((size_t)((addr)));
}


