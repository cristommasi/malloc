
#include "../include/malloc.h"


void    show_alloc_mem_internal(void) {

    t_heap *HEAP_TYPES[3] = { g_arena.tiny, g_arena.small, g_arena.large};
    size_t  total_size = 0;

    for (int i = 0; i < 3; i++) {

        t_heap *cur_heap = HEAP_TYPES[i];
        while (cur_heap != NULL) {
            
			print_heap_type(i, cur_heap);
            char *heap_addr = (char *)heap_to_chunk(cur_heap);
            char *heap_end   = heap_addr + cur_heap->total_size;

            while (heap_addr < heap_end) {

                t_chunk *cur_chunk = (t_chunk*)heap_addr;
                size_t  chunk_size = get_size(cur_chunk);

                if (chunk_size != 0 && has_flags(cur_chunk, IN_USE) && !has_flags(cur_chunk, IS_CIS)) {

                    char *data_addr         = (char*)cur_chunk;
                    char *addr_end          = (char*)cur_chunk + CHUNK_INUSE_SIZE + chunk_size;
                    size_t total_chunk_size = chunk_size  + CHUNK_INUSE_SIZE;

                    print_chunk(data_addr, addr_end, total_chunk_size);
                    total_size += total_chunk_size;
                    heap_addr = heap_addr + total_chunk_size;
                }
                else
                    heap_addr = heap_addr + CHUNK_INUSE_SIZE;
            }
            cur_heap = cur_heap->next;
        }
    }
    ft_putstr_fd("Total : ", STDOUT_FILENO);
    ft_putul_fd(total_size, STDOUT_FILENO);
    write(STDOUT_FILENO, "\n", 1);

}

void print_heap_type(int index, t_heap *cur) {

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

void print_chunk(char *start, char *end, size_t bytes) {

	if (!start || !end)
		return ;
	
    ft_puthexaddr_fd((uintptr_t)start, STDOUT_FILENO, HEX_UPPER_CASE);
    ft_putstr_fd(" - ", STDOUT_FILENO);
    ft_puthexaddr_fd((uintptr_t)end, STDOUT_FILENO, HEX_UPPER_CASE);
    ft_putstr_fd(" : ", STDOUT_FILENO);
	ft_putul_fd((uintptr_t)bytes, STDOUT_FILENO);
    ft_putstr_fd(" bytes\n", STDOUT_FILENO);
}