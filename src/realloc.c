#include "../include/malloc.h"


void    *realloc_internal(void *ptr, size_t size) {
    
    t_chunk     *chunk       = NULL;
    t_heap      *heap        = NULL;
    size_t      p_new_size   = ALIGN(size);
    size_t      cur_size     = 0;

    if (ptr == NULL) {

		pthread_mutex_unlock(&g_lock);
        ptr = malloc_internal(size);
        pthread_mutex_lock(&g_lock);
        return (ptr);
	}
    if ((chunk = data_to_chunk(ptr)) == NULL) {
		
        return (NULL);
	}
    if ((heap = arena_heap_find_by_chunk(chunk)) == NULL) {
		
        return (NULL);
	}
    if (size == 0) {
		
        free_internal(ptr);
        return (NULL);
	}
    if ((cur_size = get_size(chunk)) == p_new_size) {
		
        return (ptr);
	}
    if (heap_is_different_type(p_new_size, cur_size) || heap_type(p_new_size) == HEAP_TINY) {

        return (arena_get_new_chunk_type(ptr, p_new_size, cur_size));
	}
	else if (p_new_size != cur_size && heap_type(p_new_size) == HEAP_SMALL) {

		if ((chunk = heap_realloc_in_place(heap, chunk, p_new_size)) != NULL)
			return (chunk_to_data(chunk));
	}
    return (arena_get_new_chunk_type(ptr, p_new_size, cur_size));
}




