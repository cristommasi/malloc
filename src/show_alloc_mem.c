
#include "../include/malloc.h"


void    show_alloc_mem_internal(void) {

    t_heap *cur[HEAP_TYPE_COUNT] = { g_arena.tiny, g_arena.small, g_arena.large};
    size_t  total_size = 0;
    int     i          = 0;


    while (i < HEAP_TYPE_COUNT) {

        t_heap *cur_heap = cur[i];
        while (cur_heap != NULL) {

			print_heap_type(i, cur_heap);
            int blocks = 0;
            char *chunk = (char *)cur_heap + sizeof(t_heap);
            char *end   = (char *)chunk + cur_heap->total_size;

            while ((char *)chunk < (char *)end) {

                size_t cur_chunk_size = get_size((t_chunk *)chunk);

                if (cur_chunk_size != 0 && has_flags((t_chunk *)chunk, IN_USE) && !has_flags((t_chunk *)chunk, IS_CIS)) {

                    char *addr_start        = (char*)chunk;
                    char *addr_end          = (char*)chunk + sizeof(t_chunk) + cur_chunk_size;
                    size_t total_chunk_size = cur_chunk_size + sizeof(t_chunk);

                    print_chunk(addr_start, addr_end, total_chunk_size);
                    total_size += total_chunk_size;
                    chunk = chunk + total_chunk_size;
                    blocks += 1;
                }
                else
                    chunk = chunk + 16;
            }
            cur_heap = cur_heap->next;
        }
        i++;
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
	
    ft_puthex_fd((uintptr_t)start, STDOUT_FILENO, HEX_UPPER_CASE);
    ft_putstr_fd(" - ", STDOUT_FILENO);
    ft_puthex_fd((uintptr_t)end, STDOUT_FILENO, HEX_UPPER_CASE);
    ft_putstr_fd(" : ", STDOUT_FILENO);
	ft_putul_fd((uintptr_t)bytes, STDOUT_FILENO);
    ft_putstr_fd(" bytes\n", STDOUT_FILENO);
}