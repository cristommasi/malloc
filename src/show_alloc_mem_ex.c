#include "../include/malloc.h"


void    show_alloc_mem_ex_internal(int show_type) {


	t_heap *HEAP_TYPES[3] = { g_arena.tiny, g_arena.small, g_arena.large };
	size_t  total_size    = 0;

	if (!HEAP_TYPES[0] && !HEAP_TYPES[1] && !HEAP_TYPES[2]) return;

	for (int i = 0; i < 3; i++) {


		t_heap *heap = HEAP_TYPES[i];
		while (heap != NULL) {
			
			char *heap_addr = (char *)heap_to_chunk(heap);
			char *heap_end  = (char*)heap_addr + heap->total_size - CHUNK_INUSE_SIZE;
			size_t  alloc_size = 0;
			size_t  free_size = 0;

			print_heap_info(i, heap);
			if (show_type) heap_addr -= 16;

			while (heap_addr < heap_end) {

                t_chunk *cur_chunk = (t_chunk*)heap_addr;
				size_t  chunk_size = get_size(cur_chunk);
                
				if (chunk_size != 0 && !has_flags(cur_chunk, IN_USE) && !has_flags(cur_chunk, IS_CIS)) {

					if (show_type)
						print_data_in_chunk(cur_chunk, 16);
					free_size += chunk_size;
				}
				else if (chunk_size != 0 && has_flags(cur_chunk, IN_USE) && !has_flags(cur_chunk, IS_CIS)) {

					if (!show_type)
						print_data_in_chunk(cur_chunk, chunk_size);
					else
						print_data_in_chunk(cur_chunk, 16);
					alloc_size += chunk_size;
					heap_addr += chunk_size;
				}
				heap_addr += CHUNK_INUSE_SIZE;
			}
			total_size += alloc_size;
			print_heap_total(heap, alloc_size, free_size);
			heap = heap->next;
		}
	}
	print_total_size(total_size);
}

