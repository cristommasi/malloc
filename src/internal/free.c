
#include "../../include/malloc_internal.h"

int    free_internal(void *ptr) {
	
	t_heap  *groups[3]    = { g_arena.tiny,   g_arena.small,  g_arena.large };
	t_heap **heads[3]     = { &g_arena.tiny, &g_arena.small, &g_arena.large };
	t_heap  *heap         = NULL;
	t_chunk *chunk        = NULL;



	if (ptr == NULL)
		return (F_NO_ERROR);

	if ((chunk = data_to_chunk(ptr)) == NULL) {
		return (F_INV_PTR_ERROR);
	}
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
				if (type == HEAP_TINY && heap->blocks >= 1) {
        		    arena_fastbin_set(heap, chunk);
					update_heap_blocks(heap, -1);
				}
        		else if (type == HEAP_SMALL && heap->blocks >= 1) {
					
        		    arena_smallbin_set(heap, chunk);
					update_heap_blocks(heap, -1);
				}
        		else if (type == HEAP_LARGE && heap->blocks >= 1) {

					update_heap_blocks(heap, -1);
				}
        		if (heap->blocks == 0) {

        		    if (type == HEAP_TINY) {
						
        		        arena_fastbin_drain(heap);
					}
        		    else if (type == HEAP_SMALL) {

        		        arena_smallbin_drain(heap);
					}
        		    return (arena_heap_munmap(heap, heads[type]));
        		}
        		return (F_NO_ERROR);
			}
			heap = heap->next;
		}
	}
	return (F_NO_ERROR);
}
