
#include "../../include/malloc_internal.h"

int    free_internal(void *ptr) {
	
	t_heap  *groups[3]    = { g_arena.tiny,   g_arena.small,  g_arena.large };
	t_heap **heads[3]     = { &g_arena.tiny, &g_arena.small, &g_arena.large };
	t_heap  *heap         = NULL;
	t_chunk *chunk        = NULL;
	bool	belongs		  = false;

	if (ptr == NULL)
		return (F_NO_ERROR);

	if ((chunk = data_to_chunk(ptr)) == NULL) {
		return (F_INV_PTR_ERROR);
	}
	for (t_heap_type type = HEAP_TINY; type < HEAP_TYPE_COUNT; type++) {

		heap = groups[type];
		while (heap != NULL) {

			if (chunk_belongs_to_heap(heap, chunk)) {

				belongs = true;
				if (already_freed(chunk)) {
					return (F_DOUBLE_FREE_ERROR);
				}
				else if (is_invalid_memory(chunk)) {
					return (F_INV_PTR_ERROR);
				}
				if (heap->alloc_chunks >= 1) {

					if (type == HEAP_TINY) {

        			    arena_fastbin_set(heap, chunk);
						heap_update_alloc_chunks(heap, -1);
					}
        			else if (type == HEAP_SMALL) {

        			    arena_smallbin_set(heap, chunk);
						heap_update_alloc_chunks(heap, -1);
					}
        			else if (type == HEAP_LARGE) {

						heap_update_alloc_chunks(heap, -1);
					}
				}
        		if (heap->alloc_chunks == 0) {

        		    if (type == HEAP_TINY) {

        		        arena_fastbin_drain(heap);
					}
        		    else if (type == HEAP_SMALL) {

        		        arena_smallbin_drain(heap);
					}
					arena_heap_unlink(heap, heads[type]);
        		    return (arena_heap_munmap(heap));
        		}
        		return (F_NO_ERROR);
			}
			heap = heap->next;
		}
	}
	if (belongs == false)
		return (F_INV_PTR_ERROR);
	return (F_NO_ERROR);
}
