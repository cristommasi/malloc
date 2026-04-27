#include "../include/malloc.h"

t_arena g_arena = {0};


void    *ft_malloc(size_t size) {

	t_heap  *heap  = NULL;
	t_chunk	*chunk = NULL;
	void	*ptr   = NULL;

	if (size == 0) {
		return (NULL);
	}
	size = ALIGN(size);
	if (arena_heap_uninitialized_or_large(size)) {

		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);

		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);

	}
	else if ((chunk = arena_fastbin_get(size)) != NULL) {


		if ((heap = arena_heap_find_by_chunk(chunk)) == NULL)
			return (NULL);
		heap->blocks += 1;

	}
	else if (((chunk = heap_find_cis_mem_chunk(size)) == NULL)) {

		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);

		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);

	}
	ptr = chunk_to_data(chunk);
	return (ptr);
}

void    ft_free(void *ptr) {
	
	t_chunk *chunk        = NULL;
	t_heap *groups[3]     = { g_arena.tiny, g_arena.small, g_arena.large };
	t_heap **heads[3]     = { &g_arena.tiny, &g_arena.small, &g_arena.large };
	t_heap  *heap         = NULL;
	t_heap  *prev         = NULL;


	if (ptr == NULL)
		return ;

	if ((chunk = data_to_chunk(ptr)) == NULL)
		return ;

	for (int i = 0; i < 3; i++) {

		heap = groups[i];
		prev = NULL;
		while (heap != NULL) {

			if (chunk_belongs_to_heap(heap, chunk)) {

				if (is_large(chunk)) {

					arena_heap_munmap(prev, heap, heads[i]);
					return ;
				}
				else {

					if (!has_flags(chunk, IN_USE)) {
						
						return (void)write(2, DB_FREE_ERROR, sizeof(DB_FREE_ERROR));
					}

					if (heap->blocks > 1) {
						
						return arena_fastbin_set(heap, chunk);
					}
					else {

						arena_fastbin_drain(heap);
						return arena_heap_munmap(prev, heap, heads[i]);
					}
				}
			}
			prev = heap;
			heap = heap->next;
		}

	}
	write(2, DB_FREE_ERROR, sizeof(DB_FREE_ERROR));
}

void    *ft_realloc(void *ptr, size_t size) {
    
    t_chunk     *chunk       = NULL;
    t_heap      *heap        = NULL;
    size_t      p_new_size   = ALIGN(size);
    size_t      cur_size     = 0;

    
    if (ptr == NULL) {
		
        return (ft_malloc(size));
	}
    if ((chunk = data_to_chunk(ptr)) == NULL) {
		
        return (NULL);
	}
    if ((heap = arena_heap_find_by_chunk(chunk)) == NULL) {
		
        return (NULL);
	}
    if (size == 0) {
		
        return (ft_free(ptr), NULL);
	}
    if ((cur_size = get_size(chunk)) == p_new_size) {
		
        return (ptr);
	}
    if (heap_is_different_type(p_new_size, cur_size)) {
		
        return (arena_get_new_chunk(ptr, p_new_size, cur_size));
	}
	else if (p_new_size != cur_size) {

		
		if ((chunk = heap_realloc_in_place(heap, chunk, p_new_size)) != NULL)
			return (chunk_to_data(chunk));
	}
    return (arena_get_new_chunk(ptr, p_new_size, cur_size));

}

void    show_alloc_mem(void) {

    t_heap *cur[3]     = { g_arena.tiny, g_arena.small, g_arena.large};
    size_t  total_size = 0;
    int     i          = 0;

    while (i < 3) {

        t_heap *heap_type = cur[i];

        while (heap_type != NULL) {

            t_heap *cur_heap = heap_type;
            print_heap_type(i, cur_heap);

            while (cur_heap != NULL) {

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
                    }
                    else
                        chunk = chunk + 16;
                }

                cur_heap = cur_heap->next;
            }

            heap_type = heap_type->next;
        }
        i++;
    }
    write(1, "Total : ", sizeof("Total : "));
    ft_put_ul(total_size);
    write(1, "\n", 1);
}