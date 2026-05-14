#include "../include/malloc.h"

void	print_heap_total(t_heap *heap, size_t alloc_size, size_t free_size) {

	ft_putstr_fd("ALLOC : ", STDOUT_FILENO);
	ft_putul_fd(alloc_size, STDOUT_FILENO);
	ft_putstr_fd(" bytes\n", STDOUT_FILENO);

	ft_putstr_fd("FREE  : ", STDOUT_FILENO);
	ft_putul_fd(free_size, STDOUT_FILENO);
	ft_putstr_fd(" bytes\n", STDOUT_FILENO);

	ft_putstr_fd("CIS   : ", STDOUT_FILENO);
	ft_putul_fd(heap_free_size(heap), STDOUT_FILENO);
	ft_putstr_fd(" bytes\n", STDOUT_FILENO);
}

void	print_total_size(size_t size) {

	ft_putstr_fd("Total : ", STDOUT_FILENO);
	ft_putul_fd(size, STDOUT_FILENO);
	ft_putstr_fd(" bytes\n", STDOUT_FILENO);
}

void	print_heap_info(int i, t_heap *heap) {

	if (!heap) return;

	print_heap_type(i, heap);
	
	write(1, HEX_DUMP_HEADER_TXT, sizeof(HEX_DUMP_HEADER_TXT));
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
		}
		else {

			write(STDOUT_FILENO, "  ", 2);
		}
		if ((i + 1) % 2 == 0)
			write(STDOUT_FILENO, " ", 1);
	}
	write(STDOUT_FILENO, " ", 1);
}

void	print_data_bytes_ascii(char *data, size_t len) {
	
	for (int i = 0; i < 16; i++) {

		if (i < (int)len) {

			unsigned char byte = (unsigned char)data[i];
			if (ft_isprint(byte))
				write(STDOUT_FILENO, &byte, 1);
			else
				write(STDOUT_FILENO, ".", 1);
		}
		else {
			write(STDOUT_FILENO, " ", 1);
		}
	}
}

void    print_heap_type(int index, t_heap *cur) {

	if (!cur)
		return ;
	
	if (index == 0)
		ft_putstr_fd("TINY : ", STDOUT_FILENO);
	else if (index == 1)
		ft_putstr_fd("SMALL : ", STDOUT_FILENO);
	else if (index == 2)
		ft_putstr_fd("LARGE : ", STDOUT_FILENO);

	ft_puthexaddr_fd((uintptr_t)cur, STDOUT_FILENO, 1);
	write(STDOUT_FILENO, "\n", 1);
}

void	print_chunk_addr(t_chunk *cur_chunk, size_t chunk_size) {

	char *data_addr         = (char*)cur_chunk;
	char *addr_end          = (char*)cur_chunk + CHUNK_INUSE_SIZE + chunk_size;

	ft_puthexaddr_fd((uintptr_t)data_addr, STDOUT_FILENO, HEX_UPPER_CASE);
	ft_putstr_fd(" - ", STDOUT_FILENO);
	ft_puthexaddr_fd((uintptr_t)addr_end, STDOUT_FILENO, HEX_UPPER_CASE);
	ft_putstr_fd(" : ", STDOUT_FILENO);
	ft_putul_fd((uintptr_t)chunk_size, STDOUT_FILENO);
	ft_putstr_fd(" bytes\n", STDOUT_FILENO);
}