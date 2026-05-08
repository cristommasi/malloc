#include "../include/malloc.h"



int    free_internal(void *ptr) {
	
	t_chunk *chunk        = NULL;
	t_heap *groups[3]     = { g_arena.tiny, g_arena.small, g_arena.large };
	t_heap **heads[3]     = { &g_arena.tiny, &g_arena.small, &g_arena.large };
	t_heap  *heap         = NULL;


	if (ptr == NULL)
		return F_NO_ERROR;

	printf("free_internal\n");
	if ((chunk = data_to_chunk(ptr)) == NULL)
		return F_INV_PTR_ERROR;
	
	
	for (int i = 0; i < 3; i++) {

		heap = groups[i];
		while (heap != NULL) {

			if (!chunk_belongs_to_heap(heap, chunk)) {
				heap = heap->next;
				continue;
			}
			printf("chunk_belongs_to_heap\n");
			if (!has_flags(chunk, IN_USE) && !has_flags(chunk, IS_CIS)) {
				printf("free_internal is FREE\n");
				return F_DOUBLE_FREE_ERROR;
			}
			else if (!has_flags(chunk, IN_USE) && has_flags(chunk, IS_CIS)) {
				printf("free_internal MEM is CIS\n");
				return F_INV_PTR_ERROR;
			}
			if (heap->blocks >= 1) {

				if (i == 0) {
					arena_fastbin_set(heap, chunk);
				}
				else if (i == 1) {
					arena_smallbin_set(heap, chunk);
				}
				else if (i == 2) {
					heap->blocks = 0;
				}
				if (heap->blocks == 0)
					return arena_heap_munmap(heap, heads[i]);
			}
			heap = heap->next;
		}
	}
	return F_NO_ERROR;
}

