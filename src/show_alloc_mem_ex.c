#include "../include/malloc.h"


void    show_alloc_mem_ex_internal(int show_free_zones) {

	t_heap *HEAP_TYPES[3] = { g_arena.tiny, g_arena.small, g_arena.large};
	(void)show_free_zones;
	write(1, HEX_DUMP_HEADER_TXT, sizeof(HEX_DUMP_HEADER_TXT));

	for (int i = 0; i < 3; i++) {

		t_heap *heap = HEAP_TYPES[i];
		while (heap != NULL) {

			print_heap_type(i, heap);
			char *heap_addr = (char *)heap_to_chunk(heap);
			char *heap_end  = (char*)heap_addr + heap->total_size;
	
			while (heap_addr < heap_end) {

				ft_puthexaddr_fd((uintptr_t)heap_addr, STDOUT_FILENO, HEX_LOWER_CASE);
				ft_putstr_fd(": ", STDOUT_FILENO);
				print_data_bytes_hex(heap_addr, 16);
				print_data_bytes_ascii(heap_addr, 16);
				write(STDOUT_FILENO, "\n", 1);

				heap_addr = heap_addr + CHUNK_INUSE_SIZE;
			}
			heap = heap->next;
		}

	}
}

size_t  print_data_in_chunk(t_chunk *cur_chunk, size_t chunk_size) {

	char    *data_addr = (char *)cur_chunk + CHUNK_INUSE_SIZE;
	char    *data_end  = data_addr + chunk_size;

	while (data_addr < data_end) {

		size_t remaining = (data_end - data_addr);
		size_t to_print  = remaining < 16 ? remaining : 16;
		ft_puthexaddr_fd((uintptr_t)data_addr, STDOUT_FILENO, HEX_LOWER_CASE);
		ft_putstr_fd(": ", STDOUT_FILENO);
		print_data_bytes_hex(data_addr, to_print);
		print_data_bytes_ascii(data_addr, to_print);
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
