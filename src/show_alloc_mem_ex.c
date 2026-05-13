#include "../include/malloc.h"


void    show_alloc_mem_ex_internal(int show_type) {

	t_heap *HEAP_TYPES[3] = { g_arena.tiny, g_arena.small, g_arena.large };

	if (!HEAP_TYPES[0] && !HEAP_TYPES[1] && !HEAP_TYPES[2])
		return;
	for (int i = 0; i < 3; i++) {

		t_heap *heap = HEAP_TYPES[i];
		while (heap != NULL) {
			
			print_heap_info(i, heap);
			char *heap_addr = (char *)heap_to_chunk(heap);
			char *heap_end  = (char*)heap_addr + heap->total_size - CHUNK_INUSE_SIZE;
			if (show_type)
				heap_addr -= 16;
			while (heap_addr < heap_end) {


                t_chunk *cur_chunk = (t_chunk*)heap_addr;
                
				if (show_type) {

					print_data_in_chunk(cur_chunk, 16);
				}
				else if (!show_type) {

					size_t  chunk_size = get_size(cur_chunk);

					if (chunk_size != 0 && has_flags(cur_chunk, IN_USE) && !has_flags(cur_chunk, IS_CIS)) {
						print_data_in_chunk(cur_chunk, chunk_size);
						heap_addr = heap_addr + CHUNK_INUSE_SIZE + chunk_size;
						continue;
					}
				}
				heap_addr = heap_addr + CHUNK_INUSE_SIZE;
			}

			heap = heap->next;
		}

	}
}

void	print_heap_info(int i, t_heap *heap) {

	if (!heap) return;

	write(1, HEX_DUMP_HEADER_TXT, sizeof(HEX_DUMP_HEADER_TXT));
	print_heap_type(i, heap);
	ft_putstr_fd("\t\tCIS MEM : ", STDOUT_FILENO);
	ft_puthexaddr_fd((uintptr_t)heap->free_cis_start, STDOUT_FILENO, HEX_UPPER_CASE);
	write(STDOUT_FILENO, " (", 3);
	ft_putul_fd(heap_free_size(heap), STDOUT_FILENO);
	ft_putstr_fd(" bytes)\n", STDOUT_FILENO);
}

size_t  print_data_in_chunk(t_chunk *cur_chunk, size_t chunk_size) {

	char    *data_addr = (char *)cur_chunk + CHUNK_INUSE_SIZE;
	char    *data_end  = data_addr + chunk_size;

	while (data_addr < data_end) {

		ft_puthexaddr_fd((uintptr_t)data_addr, STDOUT_FILENO, HEX_LOWER_CASE);
		ft_putstr_fd(": ", STDOUT_FILENO);
		print_data_bytes_hex(data_addr, 16);
		print_data_bytes_ascii(data_addr, 16);
		write(STDOUT_FILENO, "\n", 1);
		data_addr = data_addr + 16;
	}
	return (0);  
}

void print_data_bytes_hex(char *data, size_t len) {

	for (int i = 0; i < 16; i++) {
		
		if (i < (int)len) {
			unsigned char byte = (unsigned char)data[i];
			ft_puthexbyte_fd(byte, STDOUT_FILENO, HEX_LOWER_CASE);
		} else {
			write(STDOUT_FILENO, "  ", 2);
		}
		if ((i + 1) % 2 == 0)
			write(STDOUT_FILENO, " ", 1);
	}
	write(STDOUT_FILENO, " ", 1);
}

void print_data_bytes_ascii(char *data, size_t len) {
	
	for (int i = 0; i < 16; i++) {
		if (i < (int)len) {
			unsigned char byte = (unsigned char)data[i];
			if (ft_isprint(byte))
				write(STDOUT_FILENO, &byte, 1);
			else
				write(STDOUT_FILENO, ".", 1);
		} else {
			write(STDOUT_FILENO, " ", 1);
		}
	}
}
