#include "../include/malloc.h"


void	*ft_realloc(void *ptr, size_t size) {

    try_mutex


}


void    *realloc_internal(void *ptr, size_t size) {
    
    t_chunk     *chunk       = NULL;
    t_heap      *heap        = NULL;
    size_t      p_new_size   = ALIGN(size);
    size_t      cur_size     = 0;

    
    if (ptr == NULL) {

		pthread_mutex_unlock(&g_arena.lock);
        return (ft_malloc(size));
	}
    if ((chunk = data_to_chunk(ptr)) == NULL) {
		
        return (NULL);
	}
    if ((heap = arena_heap_find_by_chunk(chunk)) == NULL) {
		
        return (NULL);
	}
    if (size == 0) {
		
		pthread_mutex_unlock(&g_arena.lock);
        return (ft_free(ptr), NULL);
	}
    if ((cur_size = get_size(chunk)) == p_new_size) {
		
        return (ptr);
	}
    if (heap_is_different_type(p_new_size, cur_size)) {
		
        pthread_mutex_unlock(&g_arena.lock);
        return (arena_get_new_chunk(ptr, p_new_size, cur_size));
	}
	else if (p_new_size != cur_size) {

		
		if ((chunk = heap_realloc_in_place(heap, chunk, p_new_size)) != NULL)
			return (chunk_to_data(chunk));
	}
    pthread_mutex_unlock(&g_arena.lock);
    return (arena_get_new_chunk(ptr, p_new_size, cur_size));

}




