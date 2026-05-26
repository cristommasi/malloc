
#include "../../include/malloc_internal.h"

int    free_internal(t_chunk *chunk) {
	
	t_heap  *groups[3]    = { g_arena.tiny,   g_arena.small,  g_arena.large };
	t_heap **heads[3]     = { &g_arena.tiny, &g_arena.small, &g_arena.large };
	t_heap  *heap         = NULL;

	for (t_heap_type type = HEAP_TINY; type < HEAP_TYPE_COUNT; type++) {

		heap = groups[type];
		while (heap != NULL) {

			if (chunk_belongs_to_heap(heap, chunk)) {

				if (already_freed(chunk)) {
					
					return (F_DOUBLE_FREE_ERROR);
				}
				else if (is_invalid_memory(chunk)) {

					return (F_INV_PTR_ERROR);
				}
				if (heap->alloc_chunks >= 1) {

					arena_bin_set(heap, chunk, type);
				}
        		if (heap->alloc_chunks == 0) {

					arena_heap_unlink(heap, heads[type]);
					return (arena_heap_cache_or_munmap(heap, type));
        		}
        		return (F_NO_ERROR);
			}
			heap = heap->next;
		}
	}
	return (F_NO_ERROR);
}
