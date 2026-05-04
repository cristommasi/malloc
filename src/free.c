#include "../include/malloc.h"





int    free_internal(void *ptr) {
	
	t_chunk *chunk        = NULL;
	t_heap *groups[3]     = { g_arena.tiny, g_arena.small, g_arena.large };
	t_heap **heads[3]     = { &g_arena.tiny, &g_arena.small, &g_arena.large };
	t_heap  *heap         = NULL;
	t_heap  *prev         = NULL;


	if (ptr == NULL)
		return F_INV_PTR_ERROR;

	if ((chunk = data_to_chunk(ptr)) == NULL)
		return F_INV_PTR_ERROR;

	if (has_perturb() && *(char*)ptr == get_perturb_free())
		return F_DOUBLE_FREE_ERROR;
	
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

