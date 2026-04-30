#include "../include/malloc.h"



void    *malloc_internal(size_t size) {

	t_heap  *heap  = NULL;
	t_chunk	*chunk = NULL;

	if (size == 0) {
		return (NULL);
	}
	size = ALIGN(size);
	if (arena_heap_uninitialized_or_large(size)) {

		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);

		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);

		return (chunk_to_data(chunk));

	}
	else if ((chunk = arena_fastbin_get(size)) != NULL) {

		if ((heap = arena_heap_find_by_chunk(chunk)) == NULL)
			return (NULL);
		heap->blocks += 1;

		return (chunk_to_data(chunk));
	}
	else if ((chunk = heap_find_cis_mem_chunk(size)) == NULL) {

		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);

		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);

		return (chunk_to_data(chunk));
	}
	return (chunk_to_data(chunk));
}


void	*ft_malloc(size_t size) {

	arena_try_mutex_init();
	pthread_mutex_lock(&g_arena.lock);
	void *ptr = malloc_internal(size);
	pthread_mutex_unlock(&g_arena.lock);
	arena_try_mutex_destroy();
	return (ptr);
}


void    show_alloc_mem(void) {

    int     TOTAL_TYPES = 3;
    t_heap *cur[3]     = { g_arena.tiny, g_arena.small, g_arena.large};

    size_t  total_size = 0;
    int     i          = 0;

    while (i < TOTAL_TYPES) {

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