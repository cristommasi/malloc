#include "../include/malloc.h"


void    *realloc_internal(void *ptr, size_t size) {
    
    t_chunk     *chunk       = NULL;
    t_heap      *heap        = NULL;
    size_t      cur_size     = 0;

    if (ptr == NULL) {

        ptr = malloc_internal(size);
        return (ptr);
	}
    if (size == 0) {
		
        free_internal(ptr);
        return (NULL);
	}
    if (ALIGN(size) < size) {
        
        return (NULL);
    }
    size = ALIGN(size);
    if ((chunk = data_to_chunk(ptr)) == NULL) {
		
        return (NULL);
	}
    if ((heap = arena_heap_find_by_chunk(chunk)) == NULL) {
		
        return (NULL);
	}
    cur_size = get_size(chunk);
    if (cur_size == size) {
		
        return (ptr);
	}
    if (size > cur_size && size_exceeds_rlimit(size - cur_size)) {

		return (NULL);
	}
    if (heap_is_different_type(size, cur_size) || heap_type(size) == HEAP_TINY) {

        return (arena_get_new_chunk_type(ptr, size, cur_size));
	}
	else if (size != cur_size && heap_type(size) == HEAP_SMALL) {

		if ((chunk = chunk_realloc_in_place(heap, chunk, size)) != NULL)
			return (chunk_to_data(chunk));
	}
    return (arena_get_new_chunk_type(ptr, size, cur_size));
}




