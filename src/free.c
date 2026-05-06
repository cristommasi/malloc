#include "../include/malloc.h"



int    free_internal(void *ptr) {
	
	t_chunk *chunk        = NULL;
	t_heap *groups[3]     = { g_arena.tiny, g_arena.small, g_arena.large };
	t_heap **heads[3]     = { &g_arena.tiny, &g_arena.small, &g_arena.large };
	t_heap  *heap         = NULL;


	if (ptr == NULL)
		return F_NO_ERROR;

	if ((chunk = data_to_chunk(ptr)) == NULL)
		return F_INV_PTR_ERROR;

	
	for (int i = 0; i < 3; i++) {

		heap = groups[i];
		while (heap != NULL) {

			if (chunk_belongs_to_heap(heap, chunk)) {

				if (is_large(chunk)) {

					return arena_heap_munmap(heap, heads[i]);
				}
				else {

					if (!has_flags(chunk, IN_USE)) {
						
						return F_DOUBLE_FREE_ERROR;
					}
					if (heap->blocks >= 2) {
						
						arena_fastbin_set(heap, chunk);
						return F_NO_ERROR;
					}
					else {

						arena_fastbin_set(heap, chunk);
						arena_fastbin_drain(heap);
						return arena_heap_munmap(heap, heads[i]);
					}
				}
			}
			heap = heap->next;
		}

	}
	return F_INV_PTR_ERROR;
}

