
#include "../include/malloc.h"


void    show_alloc_mem_internal(void) {

    t_heap *HEAP_TYPES[3] = { g_arena.tiny, g_arena.small, g_arena.large};
    size_t  total_size    = 0;

    if (!HEAP_TYPES[0] && !HEAP_TYPES[1] && !HEAP_TYPES[2]) return;

    for (int i = 0; i < 3; i++) {

        t_heap *cur_heap = HEAP_TYPES[i];
        while (cur_heap != NULL) {
            
            char *heap_addr = (char *)heap_to_chunk(cur_heap);
            char *heap_end   = heap_addr + cur_heap->total_size - CHUNK_INUSE_SIZE;

			print_heap_type(i, cur_heap);

            while (heap_addr < heap_end) {

                t_chunk *cur_chunk = (t_chunk*)heap_addr;
                size_t  chunk_size = get_size(cur_chunk);

                if (chunk_size != 0 && has_flags(cur_chunk, IN_USE) && !has_flags(cur_chunk, IS_CIS)) {

                    print_chunk_addr(cur_chunk, chunk_size);
                    total_size += chunk_size;
                    heap_addr += chunk_size;
                }
                heap_addr += CHUNK_INUSE_SIZE;
            }
            cur_heap = cur_heap->next;
        }
    }
    print_total_size(total_size);
}
