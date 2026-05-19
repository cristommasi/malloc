#include "../../include/malloc_internal.h"


void    *malloc_internal(size_t size) {

	t_heap  *heap  = NULL;
	t_chunk	*chunk = NULL;

	
	if (size_exceeds_rlimit(size)) {

		return (NULL);
	}
	size = (!size) ? 16 : ALIGN(size);
	if ((chunk = arena_fastbin_get(size)) != NULL) {

		if ((heap = arena_heap_find_by_chunk(chunk)) == NULL)
			return (NULL);

		heap_update_alloc_chunks(heap, 1);

		return (chunk_to_data(chunk));
	}
	else if ((chunk = arena_smallbin_get(size)) != NULL) {

		if ((heap = arena_heap_find_by_chunk(chunk)) == NULL)
			return (NULL);

		heap_update_alloc_chunks(heap, 1);

		return (chunk_to_data(chunk));
	}
	else if ((chunk = heap_find_cis_mem_chunk(size)) == NULL) {

		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);

		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);

		heap_update_alloc_chunks(heap, 1);

		return (chunk_to_data(chunk));
	}
	return (chunk_to_data(chunk));
}
