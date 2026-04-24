#include "../include/malloc.h"


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

void printheap(t_heap *heap, t_chunk *lol) {

	if (!heap) return ;
	printf("--------------------------------------------------------------------------------------------------------------\n");
	printf("\033[32m");
	printf("NEW   heap       = %s\n", (heap_type(heap->total_size / 128) == TINY_CHUNK_MAX) ? "TINY" : ((heap_type(heap->total_size / 128) == SMALL_CHUNK_MAX) ? "SMALL" : "LARGE"));
	printf("heap memory      = (t_heap) %p - (mem start) %p - (mem end) %p\n", (char*)heap, (char*)heap + sizeof(t_heap), (char*)heap + sizeof(t_heap) + heap->total_size);
	printf("heap->free_start = %p\n", (char*)heap->free_cis_start);
	printf("heap->total_size = %zu\n", heap->total_size);
	printf("heap->free_size  = %zu\n", heap_free_size(heap));
	printf("heap->blocks     = %zu\n", (size_t)heap->blocks);
	printf("heap->next       = %p\n\n", heap->next);
	printf("\033[0m");

	if (heap->free_cis_start) {
		printchunk((t_chunk*)heap->free_cis_start);

	}
	if (lol)
		printchunk(lol);
	printf("--------------------------------------------------------------------------------------------------------------\n");
}

void printchunk(t_chunk *chunk) {

	if (!chunk) return ;
	if (has_flags(chunk, IS_CIS))
		printf("\033[33m");
	else
		printf("\033[35m");
	printf("NEW chunk          = %s %s %s\n", ((has_flags(chunk, IS_LARGE)) ? "LARGE" : "TINY-SMALL"), ((has_flags(chunk, IS_CIS)) ? "IS_CIS" : ""), ((has_flags(chunk, IN_USE)) ? "IN_USE" : "IS_FREE"));
	printf("chunk memory       = (t_chunk) %p - (mem start) %p - (mem end) %p\n", (char*)chunk, (char*)chunk + sizeof(t_chunk), (char*)chunk + sizeof(t_chunk) + get_size(chunk));
	printf("chunk->prev_size   = %zu\n", get_prevsize(chunk));
	printf("chunk->next_size   = %zu\n", get_nextsize(chunk));
	printf("chunk->size        = %zu\n", get_size(chunk));
	printf("chunk->NEXT        = %p\n", chunk->next);
	printf("chunk->data        = %p\n\n", (char*)chunk + sizeof(t_chunk));
	printf("\033[0m");
}

