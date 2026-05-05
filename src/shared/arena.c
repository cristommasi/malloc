
#include "../../include/malloc.h"

int			arena_heap_munmap(t_heap *prev, t_heap *cur, t_heap **head) {

	t_heap *to_free = cur;

	if (prev)
		prev->next = cur->next;
	else
		*head = cur->next;
	
	if (has_perturb()) {
		ft_memset(heap_to_chunk(to_free) , get_perturb_free(), to_free->total_size + sizeof(t_heap));
	}
	int ret = munmap((void*)to_free, to_free->total_size + sizeof(t_heap));
	g_arena.count  = (g_arena.count >= 1) ? g_arena.count - 1 : 0;
	return (ret);
}

void		*arena_get_new_chunk_type(void *ptr, size_t p_new_size, size_t cur_size) {

	pthread_mutex_unlock(&g_lock);
	void *new_ptr = ft_malloc(p_new_size);
	pthread_mutex_lock(&g_lock);

	if (!new_ptr) {
		return (NULL);
	}
	ft_memmove(new_ptr, ptr, get_min(p_new_size, cur_size));
	pthread_mutex_unlock(&g_lock);
	ft_free(ptr);
	pthread_mutex_lock(&g_lock);
	return (new_ptr);
}

bool		arena_heap_uninitialized_or_large(size_t size) {

	size_t type = heap_type(size);
	if (type == LARGE_CHUNK_MIN)
		return (true);
	else if (!g_arena.tiny && type == TINY_CHUNK_MAX)
		return (true);
	else if (!g_arena.small && type == SMALL_CHUNK_MAX)
		return (true);
	return (false);
}

t_heap		*arena_heap_find_by_chunk(t_chunk *chunk) {

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

	size_t  index        = BIN_IDX(size);
	t_chunk	*chunk       = g_arena.fastbin[index];


	if (chunk == NULL) {
		return (NULL);
	}
	g_arena.fastbin[index] = chunk->next;
	chunk->next = NULL;
	set_flags(chunk, IN_USE);

	if (has_perturb())
		chunk_perturb(chunk, ALLOC_PERTURB);

	return (chunk);
}

void		arena_fastbin_set(t_heap *heap, t_chunk *freed_chunk) {

	size_t  size = get_size(freed_chunk);
	size_t	index = BIN_IDX(size);

	
	unset_flags(freed_chunk, IN_USE);
	freed_chunk->next = g_arena.fastbin[index];
	g_arena.fastbin[index] = freed_chunk;

	if (heap->blocks == 0)
		heap->blocks = 0;
	else
		heap->blocks -= 1;
	
	t_chunk *next = get_next_chunk(heap, freed_chunk);
	if (next)
		set_prevsize(next, get_size(freed_chunk));
	t_chunk *prev = get_prev_chunk(heap, freed_chunk);
	if (prev)
		set_nextsize(prev, get_size(freed_chunk));
	
	
	if (has_perturb())
		chunk_perturb(freed_chunk, FREE_PERTURB);

}

void		arena_fastbin_drain(t_heap *heap) {

	int			i;
	t_chunk		*cur;
	t_chunk		*next;
	t_chunk		**node;

	i = 0;
	while (i < FASTBIN_COUNT)
	{
		node = &g_arena.fastbin[i];
		while (*node)
		{
			cur = *node;

			if (chunk_belongs_to_heap(heap, cur))
			{
				next = cur->next;
				cur->next = NULL;
				*node = next;
			}
			else
				node = &(*node)->next;
		}
		i++;
	}
}

void		arena_fastbin_unlink(t_chunk *chunk) {

	size_t  index        = BIN_IDX(get_size(chunk));
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


