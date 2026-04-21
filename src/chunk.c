#include "../include/malloc.h"

bool    chunk_covers_entire_heap(t_heap *heap, t_chunk *chunk) {

	return (heap->total_size == (chunk->size + sizeof(t_chunk) + heap_cis_mem_size(heap)));
}


bool    chunk_belongs_to_heap(t_heap *heap, t_chunk *chunk) {

	char *start = (char*)(heap + 1);
	char *end = start + heap->total_size;

	return ((char *)chunk >= start && (char *)chunk <= end);
}


t_chunk *next_chunk(t_heap *heap, t_chunk *chunk) {

	char *addr = ((char*)chunk + sizeof(t_chunk) + chunk->size);

	if (addr > (char*)heap + sizeof(t_heap) + heap->total_size)
		return (NULL);
	return ( (t_chunk *)((char*)chunk + sizeof(t_chunk) + chunk->size) )
}

void    *chunk_to_data(t_chunk *chunk_addr) {

	return ((void *)(chunk_addr + 1));
}


t_chunk    *data_to_chunk(void *data_addr) {

	return ((t_chunk *)data_addr - 1);
}