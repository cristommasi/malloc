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
	
	printf("1111\n");
	for (int i = 0; i < 3; i++) {

		heap = groups[i];
		while (heap != NULL) {

			if (!chunk_belongs_to_heap(heap, chunk)) {
				printf("22222\n");
				heap = heap->next;
				continue;
			}

			if (!has_flags(chunk, IN_USE) && !has_flags(chunk, IS_CIS)) {
				printf("33333\n");
				return F_DOUBLE_FREE_ERROR;
			}
			else if (!has_flags(chunk, IN_USE) && has_flags(chunk, IS_CIS)) {
				printf("44444\n");
				return F_INV_PTR_ERROR;
			}
			if (heap->blocks >= 1) {

				if (i == 0) {
					printf("5555\n");
					arena_fastbin_set(heap, chunk);
				}
				else if (i == 1) {
					printf("66666\n");
					arena_smallbin_set(heap, chunk);
				}
				else if (i == 2) {
					printf("77777\n");
					heap->blocks = 0;
				}
				if (heap->blocks == 0) {
					printf("8888\n");
					return arena_heap_munmap(heap, heads[i]);
				}
			}
			heap = heap->next;
		}
	}
	printf("9999\n");
	return F_NO_ERROR;
}

