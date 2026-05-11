#include "../include/malloc.h"



void    free_internal(void *ptr) {
	
	t_chunk *chunk        = NULL;
	t_heap *groups[3]     = { g_arena.tiny, g_arena.small, g_arena.large };
	t_heap **heads[3]     = { &g_arena.tiny, &g_arena.small, &g_arena.large };
	t_heap  *heap         = NULL;


	if (ptr == NULL)
		return free_exit(F_NO_ERROR);

	
	if ((chunk = data_to_chunk(ptr)) == NULL)
		return free_exit(F_INV_PTR_ERROR);
	
	for (int i = 0; i < 3; i++) {

		heap = groups[i];
		while (heap != NULL) {

			if (!chunk_belongs_to_heap(heap, chunk)) {

				heap = heap->next;
				continue;
			}

			if (!has_flags(chunk, IN_USE) && !has_flags(chunk, IS_CIS)) {

				return free_exit(F_DOUBLE_FREE_ERROR);
			}
			else if (!has_flags(chunk, IN_USE) && has_flags(chunk, IS_CIS)) {

				return free_exit(F_INV_PTR_ERROR);
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
				if (heap->blocks == 0) {

					return free_exit(arena_heap_munmap(heap, heads[i]));
				}
			}
			heap = heap->next;
		}
	}
	return free_exit(F_NO_ERROR);
}


void	free_exit(int err) {

    uint8_t check = get_check();
    if (err == -1) {

        if (check == 1)
            write(2, F_MUNMAP_MSG, sizeof(F_MUNMAP_MSG));
        if (check == 2)
            abort();
        else if (check == 3) {
            write(2, F_MUNMAP_MSG, sizeof(F_MUNMAP_MSG));
            abort();
        }
    }
	if (err == F_DOUBLE_FREE_ERROR) {

        if (check == 1)
            write(2, F_DOUBLE_FREE_MSG, sizeof(F_DOUBLE_FREE_MSG));
        if (check == 2)
            abort();
        else if (check == 3) {
            write(2, F_DOUBLE_FREE_MSG, sizeof(F_DOUBLE_FREE_MSG));
            abort();
        }
	}
	else if (err == F_INV_PTR_ERROR) {

        if (check == 1)
            write(2, F_INV_PTR_MSG, sizeof(F_INV_PTR_MSG));
        if (check == 2)
            abort();
        else if (check == 3) {
            write(2, F_INV_PTR_MSG, sizeof(F_INV_PTR_MSG));
            abort();
        }
	}

}
