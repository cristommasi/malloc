#include "../include/malloc.h"

bool    chunk_covers_entire_heap(t_heap *heap, t_chunk *chunk) {

	return (heap->total_size == (chunk->size + sizeof(t_chunk) + heap_cis_mem_size(heap)));
}


bool    chunk_belongs_to_heap(t_heap *heap, t_chunk *chunk) {

	char *start = (char*)(heap + 1);
	char *end = start + heap->total_size;

	return ((char *)chunk >= start && (char *)chunk <= end);
}