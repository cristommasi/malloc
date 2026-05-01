
#include "../include/malloc.h"



void    show_alloc_mem_internal(void) {

    if (!g_arena.tiny && !g_arena.small && !g_arena.large)
        return ;

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
            //DELETE AFTER
			printf("%s%p - %p : %zu bytes (free cis mem) - blocks %d%s\n", 
				YELLOW, 
				cur_heap->free_cis_start, 
				((cur_heap->free_cis_start) ? (char*)cur_heap->free_cis_start + heap_cis_mem_size(cur_heap) : NULL), 
				heap_cis_mem_size(cur_heap),
				blocks,
				RESET
			);
            cur_heap = cur_heap->next;
        }
        i++;
    }
    write(1, "Total : ", sizeof("Total : "));
    ft_put_ul(total_size);
    write(1, "\n", 1);
}

void print_heap_type(int index, t_heap *cur) {

	if (!cur)
		return ;
	
	if (index == 0)
		write(1, "TINY : ", sizeof("TINY : "));
	else if (index == 1)
		write(1, "SMALL : ", sizeof("SMALL : "));
	else if (index == 2)
		write(1, "LARGE : ", sizeof("LARGE : "));

	ft_put_hex((unsigned long )cur);
	write(1, "\n", 1);
}

void print_chunk(char *start, char *end, size_t bytes) {

	if (!start || !end)
		return ;
	
	ft_put_hex((unsigned long)start);
	write(1, " - ", sizeof(" - "));
	ft_put_hex((unsigned long)end);
	write(1, " : ", sizeof(" : "));
	ft_put_ul((unsigned long)bytes);
	write(1, " bytes\n", sizeof(" bytes\n"));
}