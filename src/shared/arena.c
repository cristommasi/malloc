
#include "../../include/malloc.h"

int			arena_heap_munmap(t_heap *to_free, t_heap **head) {

	arena_heap_unlink(to_free, head);
	 
	if (has_perturb()) {
		ft_memset(to_free, get_perturb_free(), to_free->total_size + sizeof(t_heap));
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
   t_heap  **heap = arena_heap_group_by_chunk(get_size(chunk));

   while (*heap != NULL) {

		if (chunk_belongs_to_heap(*heap, chunk))
			return (*heap);
		heap = &(*heap)->next;
   }
   return (NULL);
}

int		bin_idx(size_t size) {

	if (size == 16)
		return (0);
	int index = ((int)size - FASTBIN_MIN_CHUNK) / ALIGNMENT;
	if (index >= 64)
		return (-1);
	return (index);
}


t_chunk     *arena_fastbin_get(size_t size) {

	int  index        = bin_idx(size);
	if (index == -1)
		return (NULL);
	t_chunk	*chunk       = g_arena.fastbin[index];


	if (chunk == NULL) {
		return (NULL);
	}
	g_arena.fastbin[index] = chunk->next;
	if (g_arena.fastbin[index])
		g_arena.fastbin[index]->prev = NULL;
	chunk->next = NULL;
	chunk->prev = NULL;
	set_flags(chunk, IN_USE);

	if (has_perturb())
		chunk_perturb(chunk, ALLOC_PERTURB);

	return (chunk);
}

void		arena_fastbin_set(t_heap *heap, t_chunk *freed_chunk) {

	size_t  size = get_size(freed_chunk);
	int	index = bin_idx(size);

	if (index == -1) return ;

	unset_flags(freed_chunk, IN_USE);
	freed_chunk->prev = NULL;
	freed_chunk->next = g_arena.fastbin[index];
	if (g_arena.fastbin[index])
		g_arena.fastbin[index]->prev = freed_chunk;
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

void		arena_fastbin_unlink(t_chunk *chunk) {

	if (chunk->prev) {

		chunk->prev->next = chunk->next;
	}
	else {

		g_arena.fastbin[bin_idx(get_size(chunk))] = chunk->next;

	}
	if (chunk->next)
		chunk->next->prev = chunk->prev;

	chunk->next = NULL;
	chunk->prev = NULL;
}

void		arena_heap_unlink(t_heap *heap, t_heap **head) {


	if (heap->prev) {

		heap->prev->next = heap->next;
	}
	else {

		*head = heap->next;

	}
	if (heap->next)
		heap->next->prev = heap->prev;

	heap->next = NULL;
	heap->prev = NULL;
}


void		arena_fastbin_drain(t_heap *heap) {

	char		*cur = (char*)heap_to_chunk(heap);
	char		*end = cur + heap->total_size;

	while (cur < end)
	{

		t_chunk *chunk = (t_chunk*)cur;

		if (has_flags(chunk, IS_CIS))
			break ;
		if (!has_flags(chunk, IN_USE)) {
			arena_fastbin_unlink(chunk);
		}
		cur = cur + CHUNK_INUSE_SIZE + get_size(chunk);
	}
}



t_heap      **arena_heap_group_by_chunk(size_t size) {

	size_t zone_size = heap_page_size(size);

	if (size >= get_mmap_threshold()) {

		return (&g_arena.large);
	}
	else if (zone_size == TINY_HEAP_SIZE) {

		return (&g_arena.tiny);
	}
	else if (zone_size == SMALL_HEAP_SIZE) {

		return (&g_arena.small);
	}
	return (NULL);
}


