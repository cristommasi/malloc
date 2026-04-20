
#include "../include/malloc.h"


bool        arena_heap_uninitialized_or_large(size_t size) {

	t_heap_type TYPE = heap_type(size);


	if ((TYPE == LARGE_HEAP) || (!g_arena.tiny && TYPE == TINY_HEAP) || (!g_arena.small && TYPE == SMALL_HEAP)) {
		// if (TYPE == LARGE_HEAP)
			//printf("%s - Request mem alloc is LARGE\n", __func__);
		// else
			//printf("%s - No heaps found initially\n", __func__);
		return (true);
	}

	//printf("%s - %s Heaps found initially\n", __func__, (TYPE == TINY_HEAP) ? "TINY" : "SMALL");
	return (false);
}


t_chunk     *arena_fastbin_get(size_t size) {

	size_t  index        = BIN_IDX(size + sizeof(t_chunk));
	t_chunk *chunk       = g_arena.fastbin[index];
	t_heap *heap         = NULL;

	if (!chunk) {
		//printf("%s - Didnt find any free chunks in fastbin\n", __func__);
		return (NULL);
	}
	heap = arena_heap_find_by_chunk(chunk);
	if (!heap)
		return (NULL);
	heap->blocks += 1;
	g_arena.fastbin[index] = chunk->next;
	chunk->next = NULL;
	UNSET_INBIN(chunk);
	SET_INUSE(chunk);
	//printf("%s - Found free chunk in fastbin\n", __func__);
	return (chunk);
}

void    arena_fastbin_set(t_heap *heap, t_chunk *freed_chunk) {

	size_t	index = BIN_IDX(REAL_SIZE(freed_chunk->size) + sizeof(t_chunk));

	VALGRIND_FREELIKE_BLOCK(chunk_to_data(freed_chunk), 0);
	UNSET_INUSE(freed_chunk);
	SET_INBIN(freed_chunk);
	freed_chunk->next = g_arena.fastbin[index];
	g_arena.fastbin[index] = freed_chunk;
	heap->blocks -= 1;
	// printf("%s - Set free chunk to fastbin\n", __func__);
}

void	arena_fastbin_drain(t_heap *heap) {

	for (int i = 0; i < FASTBIN_COUNT; i++) {

		t_chunk **node = &g_arena.fastbin[i];
		while (*node != NULL) {

			if (arena_chunk_in_bounds_of_heap(heap, *node))
				*node = (*node)->next;
			else
				node = &(*node)->next;
		}
	}
	//printf("%s - Unlinked all chunks in fastbin\n", __func__);
}


int     arena_heap_munmap(t_heap *prev, t_heap *cur, t_heap **head) {

	t_heap *to_free = cur;

	if (prev)
		prev->next = cur->next;
	else
		*head = cur->next;
	return (munmap((void*)to_free, to_free->total_size + sizeof(t_heap)));
}


t_heap      **arena_heap_group(size_t size) {

	size_t zone_size = heap_page_size(REAL_SIZE(size));

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

t_heap  *arena_heap_find_by_chunk(t_chunk *chunk) {

   t_heap  **heap = arena_heap_group(REAL_SIZE(chunk->size));

   while (*heap != NULL) {

		if (chunk_belongs_to_heap(*heap, chunk))
			return (*heap);
		heap = &(*heap)->next;
   }
   return (NULL);
}


