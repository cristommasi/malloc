#include "../include/malloc.h"


void    *malloc_internal(size_t size) {

	t_heap  *heap  = NULL;
	t_chunk	*chunk = NULL;

	if (size == 0) {
		return (NULL);
	}
	size = ALIGN(size);

	if (arena_heap_uninitialized_or_large(size)) {


		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);

	
		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);
	
		heap->blocks += 1;

		return (chunk_to_data(chunk));

	}
	else if ((chunk = arena_fastbin_get(size)) != NULL) {

	
		if ((heap = arena_heap_find_by_chunk(chunk)) == NULL)
			return (NULL);

		heap->blocks += 1;

		return (chunk_to_data(chunk));
	}
	else if ((chunk = heap_find_cis_mem_chunk(size)) == NULL) {


		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);

		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);
		heap->blocks += 1;
		return (chunk_to_data(chunk));
	}
	return (chunk_to_data(chunk));
}
