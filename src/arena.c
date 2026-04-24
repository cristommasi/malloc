
#include "../include/malloc.h"


bool        arena_heap_uninitialized_or_large(size_t size) {

	return ((!g_arena.tiny && heap_type(size) == TINY_CHUNK_MAX) || (!g_arena.small && heap_type(size) == SMALL_CHUNK_MAX) || heap_type(size) == LARGE_CHUNK_MIN);
}



t_heap  *arena_heap_find_by_chunk(t_chunk *chunk) {

	if (!chunk)
		return (NULL);
   t_heap  **heap = arena_heap_group(get_size(chunk));

   while (*heap != NULL) {

		if (chunk_belongs_to_heap(*heap, chunk))
			return (*heap);
		heap = &(*heap)->next;
   }
   return (NULL);
}

t_chunk     *arena_fastbin_get(size_t size) {

	size_t  index        = BIN_IDX(size + sizeof(t_chunk));
	t_chunk *chunk       = g_arena.fastbin[index];

	if (!chunk) {
		return (NULL);
	}
	g_arena.fastbin[index] = chunk->next;
	chunk->next = NULL;
	set_flags(chunk, IN_USE);
	return (chunk);
}

void    arena_fastbin_set(t_heap *heap, t_chunk *freed_chunk) {

	size_t  size = get_size(freed_chunk);
	size_t	index = BIN_IDX(size + sizeof(t_chunk));


	unset_flags(freed_chunk, IN_USE);
	freed_chunk->next = g_arena.fastbin[index];
	g_arena.fastbin[index] = freed_chunk;
	heap->blocks -= 1;
	t_chunk *next = get_next_chunk(heap, freed_chunk);
	if (next != NULL)
		set_prevsize(next, size);
	t_chunk *prev = get_prev_chunk(heap, freed_chunk);
	if (prev != NULL)
		set_nextsize(prev, size);

}

void	arena_fastbin_drain(t_heap *heap) {

	for (int i = 0; i < FASTBIN_COUNT; i++) {

		t_chunk **node = &g_arena.fastbin[i];
		while (*node != NULL) {

			if (chunk_belongs_to_heap(heap, *node))
				*node = (*node)->next;
			else
				node = &(*node)->next;
		}
	}

}

void	arena_fastbin_unlink(t_chunk *chunk) {

	size_t  index        = BIN_IDX(get_size(chunk) + sizeof(t_chunk));
	t_chunk *cur		 = g_arena.fastbin[index];
	t_chunk *prev        = NULL;

	while (cur != NULL) {

		if (cur == chunk) {
			if (prev)
				prev->next = cur->next;
			else
				g_arena.fastbin[index] = cur->next;
			cur->next = NULL;
			return ;
		}
		prev = cur;
		cur = cur->next;
	}
}

void     arena_heap_munmap(t_heap *prev, t_heap *cur, t_heap **head) {

	t_heap *to_free = cur;

	if (prev)
		prev->next = cur->next;
	else
		*head = cur->next;
	
	if (munmap((void*)to_free, to_free->total_size + sizeof(t_heap)) == -1)
		write(2, MUNMAP_ERROR, sizeof(MUNMAP_ERROR));
}

t_heap      **arena_heap_group(size_t size) {

	size_t zone_size = heap_page_size(size);

	if (zone_size == TINY_HEAP_SIZE) {

		return (&g_arena.tiny);
	}
	else if (zone_size == SMALL_HEAP_SIZE) {

		return (&g_arena.small);
	}
	else {
		return (&g_arena.large);
	}
}

void *arena_get_new_chunk(void *ptr, size_t p_new_size, size_t cur_size) {

	void *new_ptr = ft_malloc(p_new_size);

    if (!new_ptr)
        return (NULL);
    ft_memmove(new_ptr, ptr, get_min(p_new_size, cur_size));
    ft_free(ptr);
    return (new_ptr);
}

