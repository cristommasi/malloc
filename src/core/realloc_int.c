
#include "../malloc_internal.h"

void    *realloc_internal(void *ptr, t_chunk *chunk, size_t old_size, size_t size) {
    

    t_heap      *heap        = NULL;

    if ((heap = arena_heap_find_by_chunk(chunk)) == NULL) {
		
        return (NULL);
	}
    if (old_size == size) {
		
        return (ptr);
	}
    if (heap_is_different_type(size, old_size) || heap_type(size) == HEAP_TINY) {

        return (arena_get_new_chunk_type(ptr, size, old_size));
	}
	else if (size != old_size && heap_type(size) == HEAP_SMALL) {

		if ((chunk = chunk_realloc_in_place(heap, chunk, size)) != NULL)
			return (chunk_to_data(chunk));
	}
    return (arena_get_new_chunk_type(ptr, size, old_size));
}


