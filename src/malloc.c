#include "../include/malloc.h"


void    *malloc_internal(size_t size) {

	t_heap  *heap  = NULL;
	t_chunk	*chunk = NULL;

	if (size == 0)
		size = 16;
	printf("malloc size = %zu\n", size);
	size = ALIGN(size);
	if ((chunk = arena_fastbin_get(size)) != NULL) {
		printf("arena_fastbin_get  != NULL\n");
		if ((heap = arena_heap_find_by_chunk(chunk)) == NULL)
			return (NULL);

		heap->blocks += 1;

		return (chunk_to_data(chunk));
	}
	else if ((chunk = arena_smallbin_get(size)) != NULL) {

		printf("arena_smallbin_get  != NULL\n");
		if ((heap = arena_heap_find_by_chunk(chunk)) == NULL)
			return (NULL);

		heap->blocks += 1;

		return (chunk_to_data(chunk));
	}
	else if ((chunk = heap_find_cis_mem_chunk(size)) == NULL) {

		printf("heap_find_cis_mem_chunk  == NULL\n");
		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);
		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);
		heap->blocks += 1;

		return (chunk_to_data(chunk));
	}
	return (chunk_to_data(chunk));
}
