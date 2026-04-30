#include "../include/malloc.h"


void	ft_free(void *ptr) {

	arena_try_mutex_init();
	pthread_mutex_lock(&g_arena.lock);

	int ret = free_internal(ptr);
    if (ret == -1) {

        write(2, F_MUNMAP_MSG, sizeof(F_MUNMAP_MSG));
    }
	if (ret == F_DOUBLE_FREE_ERROR) {

		write(2, F_DOUBLE_FREE_MSG, sizeof(F_DOUBLE_FREE_MSG));
	}
	else if (ret == F_INV_PTR_ERROR) {

		write(2, F_INV_PTR_MSG, sizeof(F_INV_PTR_MSG));
	}
	pthread_mutex_unlock(&g_arena.lock);

	arena_try_mutex_destroy();
}


int    free_internal(void *ptr) {
	
	t_chunk *chunk        = NULL;
	t_heap *groups[3]     = { g_arena.tiny, g_arena.small, g_arena.large };
	t_heap **heads[3]     = { &g_arena.tiny, &g_arena.small, &g_arena.large };
	t_heap  *heap         = NULL;
	t_heap  *prev         = NULL;


	if (ptr == NULL)
		return F_INV_PTR_MSG;

	if ((chunk = data_to_chunk(ptr)) == NULL)
		return F_INV_PTR_MSG;

	for (int i = 0; i < 3; i++) {

		heap = groups[i];
		prev = NULL;
		while (heap != NULL) {

			if (chunk_belongs_to_heap(heap, chunk)) {

				if (is_large(chunk)) {

					return arena_heap_munmap(prev, heap, heads[i]);
				}
				else {

					if (!has_flags(chunk, IN_USE)) {
						
						return F_DOUBLE_FREE_ERROR;
					}
					if (heap->blocks > 1) {
						
						arena_fastbin_set(heap, chunk);
						return F_NO_ERROR;
					}
					else {

						arena_fastbin_drain(heap);
						return arena_heap_munmap(prev, heap, heads[i]);
					}
				}
			}
			prev = heap;
			heap = heap->next;
		}

	}
	return F_INV_PTR_ERROR;
}

