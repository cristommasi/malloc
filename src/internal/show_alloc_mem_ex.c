
#include "../../include/malloc_internal.h"


void    show_alloc_mem_ex_internal(void) {


	t_heap *HEAP_TYPES[3] = { g_arena.tiny, g_arena.small, g_arena.large };
	size_t  total_size    = 0;
	int     show_type = get_show_info();

	if (!HEAP_TYPES[0] && !HEAP_TYPES[1] && !HEAP_TYPES[2])
		return;
	
	for (int i = 0; i < HEAP_TYPE_COUNT; i++) {

		t_heap *heap = HEAP_TYPES[i];

		while (heap != NULL) {
			
			char	*heap_addr = (char *)heap_to_chunk(heap);
			char	*heap_end  = (char*)heap_addr + heap->total_size - CHUNK_INUSE_SIZE;
			size_t  alloc_size = 0;
			size_t  free_size  = 0;

			print_heap_type(i, heap);
			print_string(HEX_DUMP_HEADER_TXT);

			while (heap_addr < heap_end) {

			    t_chunk *cur_chunk  = (t_chunk*)heap_addr;
			    size_t   chunk_size = get_size(cur_chunk);
			
			    if (chunk_size == 0) {
			        heap_addr += CHUNK_INUSE_SIZE;
			        continue;
			    }
			    if (has_flags(cur_chunk, IN_USE) && !has_flags(cur_chunk, IS_CIS))
			    {
			        if (show_type)
			            print_data_in_chunk((char*)cur_chunk, chunk_size + CHUNK_INUSE_SIZE, 0);
			        else
			            print_data_in_chunk((char*)cur_chunk, chunk_size, 16);
			        alloc_size += chunk_size;
			    }
			    else if (has_flags(cur_chunk, IS_CIS))
			    {
			        if (show_type)
			            print_data_in_chunk((char*)cur_chunk, chunk_size + CHUNK_INUSE_SIZE, 0);
					free_size += chunk_size + 16;
			    }
			    else
			    {
			        if (show_type)
			            print_data_in_chunk((char*)cur_chunk, chunk_size + CHUNK_INUSE_SIZE, 0);
					free_size += chunk_size + 16;
			    }
			    heap_addr += chunk_size + CHUNK_INUSE_SIZE;
			}
			total_size += alloc_size;
			if (show_type) {
				print_string("ALLOC : ");print_long(alloc_size);print_string(" bytes\n");
				print_string("FREE  : ");print_long(free_size);print_string(" bytes\n");
			}
			heap = heap->next;
		}
	}
	if (show_type) {
		print_string("Total : ");print_long(total_size);print_string(" bytes\n");
	}
}

