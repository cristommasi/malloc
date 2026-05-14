
#include "../../include/malloc.h"

int			arena_heap_munmap(t_heap *to_free, t_heap **head) {

	arena_heap_unlink(to_free, head);
	 
	size_t total = to_free->total_size + sizeof(t_heap);
	if (has_perturb()) {
		ft_memset(to_free, get_perturb_free(), total);
	}
	int ret = munmap((void*)to_free, total);
	g_arena.count  = (g_arena.count >= 1) ? g_arena.count - 1 : 0;
	return (ret);
}

void		*arena_get_new_chunk_type(void *ptr, size_t p_new_size, size_t cur_size) {

	void *new_ptr = malloc_internal(p_new_size);
	if (!new_ptr) {
		return (NULL);
	}
	ft_memmove(new_ptr, ptr, get_min(p_new_size, cur_size));
	free_internal(ptr);
	return (new_ptr);
}

bool		arena_heap_uninitialized_or_large(size_t size) {

	t_heap_type type = heap_type(size);
	if (type == HEAP_LARGE)
		return (true);
	else if (!g_arena.tiny && type == HEAP_TINY)
		return (true);
	else if (!g_arena.small && type == HEAP_SMALL)
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

int			FBIN_IDX(size_t size) {

	if (size >= SMALLBIN_MIN_CHUNK) return (-1);
	if (size == FASTBIN_MIN_CHUNK) return (0);
	return (((int)size - FASTBIN_MIN_CHUNK) / ALIGNMENT);
}

int			SBIN_IDX(size_t size) {

	if (size < SMALLBIN_MIN_CHUNK) return (-1);
	if (size == SMALLBIN_MIN_CHUNK) return (0);
	int index = ((int)size - SMALLBIN_MIN_CHUNK) / ALIGNMENT;
	if (index >= 56) return (-1);
	return (index);
}

t_chunk     *arena_fastbin_get(size_t size) {
	
	int  index= FBIN_IDX(size);
	if (index == -1) {
		return (NULL);
	}

	t_chunk	*chunk       = g_arena.fastbin[index];
	if (chunk == NULL) return (NULL);


	g_arena.fastbin[index] = chunk->next;
	if (g_arena.fastbin[index])
		g_arena.fastbin[index]->prev = NULL;
	chunk->next = NULL;
	chunk->prev = NULL;
	set_flags(chunk, IN_USE);

	if (has_perturb())
		ft_memset((char*)chunk + CHUNK_INUSE_SIZE, get_perturb_alloc(), get_size(chunk));

	return (chunk);
}

void		arena_fastbin_set(t_heap *heap, t_chunk *freed_chunk) {

	size_t  size;
	int		index;
	t_chunk *next = NULL;


	if (!freed_chunk)
		return ;
	size = get_size(freed_chunk);
	index =  FBIN_IDX(size);
	if (index == -1)
		return ;
	unset_flags(freed_chunk, IN_USE);
	if ((next = get_next_chunk(heap, freed_chunk)) != NULL)
		set_prevsize(next, size);
		
	freed_chunk->prev = NULL;
	freed_chunk->next = g_arena.fastbin[index];
	if (g_arena.fastbin[index])
		g_arena.fastbin[index]->prev = freed_chunk;
	g_arena.fastbin[index] = freed_chunk;

	if (heap->blocks >= 1)
		heap->blocks -= 1;

	if (has_perturb())
		ft_memset((char*)freed_chunk + CHUNK_FREE_SIZE, get_perturb_free(), get_size(freed_chunk) - 16);

}

void		arena_fastbin_unlink(t_chunk *chunk) {

	if (chunk->prev) {

		chunk->prev->next = chunk->next;
	}
	else {

		size_t size = get_size(chunk);
		if (size != 0)
			g_arena.fastbin[FBIN_IDX(size)] = chunk->next;

	}
	if (chunk->next)
		chunk->next->prev = chunk->prev;

	chunk->next = NULL;
	chunk->prev = NULL;
}

void		arena_fastbin_drain(t_heap *heap) {

	char		*cur = (char*)heap_to_chunk(heap);
	char		*end = cur + heap->total_size - heap_free_size(heap);

	while (cur < end)
	{
		t_chunk *chunk = (t_chunk*)cur;
		if (has_flags(chunk, IS_CIS)) {
			break ;
		}
		if (!has_flags(chunk, IN_USE)) {
			arena_fastbin_unlink(chunk);
		}
		cur += CHUNK_INUSE_SIZE + get_size(chunk);
		
	}
}


t_chunk		*arena_smallbin_get(size_t size) {

	int		index;
	t_chunk	*head = NULL;
	t_chunk	*tail = NULL;

	index = SBIN_IDX(size);
	if (index == -1)
		return (NULL);

	if (!g_arena.smallbin[index]) {

		return (NULL);
	}
	head = g_arena.smallbin[index];
	if (head->next == head) {

		g_arena.smallbin[index] = NULL;
		head->next = NULL;
		head->prev = NULL;
		set_flags(head, IN_USE);
		if (has_perturb())
			ft_memset((char*)head + CHUNK_INUSE_SIZE, get_perturb_alloc(), get_size(head));
		return (head);
	}

	tail = head->prev;
	tail->prev->next = head;
	head->prev = tail->prev;
	tail->next = NULL;
	tail->prev = NULL;
	set_flags(tail, IN_USE);

	if (has_perturb())
		ft_memset((char*)tail + CHUNK_INUSE_SIZE, get_perturb_alloc(), get_size(tail));
	return (tail);
}

void		arena_smallbin_set(t_heap *heap, t_chunk *freed_chunk) {

	size_t		size;
	int			index;
	t_chunk		*head = NULL;
	t_chunk		*tail = NULL;


	if (!heap || !freed_chunk)
		return ;

	if ((freed_chunk = chunk_coalesce(heap, freed_chunk)) == NULL)
		return ;

	size = get_size(freed_chunk);
	if ((index = SBIN_IDX(size)) == -1)
		return ;
	unset_flags(freed_chunk, IN_USE);
	if (!g_arena.smallbin[index]) {
		g_arena.smallbin[index] = freed_chunk;
		freed_chunk->next = freed_chunk;
		freed_chunk->prev = freed_chunk;
	}
	else {
		head = g_arena.smallbin[index];
		tail = head->prev;
		freed_chunk->next = head;
		freed_chunk->prev = tail;
		tail->next = freed_chunk;
		head->prev = freed_chunk;
		g_arena.smallbin[index] = freed_chunk;
	}
	
	heap->blocks = (heap->blocks >= 1) ? heap->blocks - 1 : 0;
	if (has_perturb()) {
		ft_memset((char*)freed_chunk + CHUNK_FREE_SIZE, get_perturb_free(), get_size(freed_chunk) - 16);
	}
}

void		arena_smallbin_unlink(t_chunk *chunk) {

	if (!chunk) return ;
	size_t size = get_size(chunk);
	if (chunk->next == chunk) {

		chunk->next = NULL;
		chunk->prev = NULL;
		g_arena.smallbin[SBIN_IDX(size)] = NULL;
		return ;
	}
	chunk->prev->next = chunk->next;
	chunk->next->prev = chunk->prev;
	g_arena.smallbin[SBIN_IDX(size)] = chunk->next;
	chunk->next = NULL;
	chunk->prev = NULL;
}

void		arena_smallbin_drain(t_heap *heap) {

	char		*cur = (char*)heap_to_chunk(heap);
	char		*end = cur + heap->total_size - heap_free_size(heap);

	while (cur < end)
	{
		t_chunk *chunk = (t_chunk*)cur;
		if (has_flags(chunk, IS_CIS)) {
			break ;
		}
		if (!has_flags(chunk, IN_USE)) {
			arena_smallbin_unlink(chunk);
		}
		cur += CHUNK_INUSE_SIZE + get_size(chunk);
	}
}

void		arena_heap_unlink(t_heap *heap, t_heap **head) {

	if (!heap || !head || !*head)
		return ;


	if (heap->prev)
		heap->prev->next = heap->next;
	else
		*head = heap->next;

	if (heap->next)
		heap->next->prev = heap->prev;

	heap->next = NULL;
	heap->prev = NULL;
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


