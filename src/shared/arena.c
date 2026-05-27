
#include "../malloc_internal.h"

int			arena_heap_munmap(t_heap *to_free) {

	size_t total = to_free->total_size + sizeof(t_heap);
	
	if (has_perturb()) {
		do_perturb(to_free, get_perturb_free(), total);
	}
	
	int ret = munmap((void*)to_free, total);
	if (ret != -1) {

		update_arena_heap_count(-1);
	}
	return (ret);
}

int			arena_heap_cache_or_munmap(t_heap *to_free, t_heap_type type) {

	if (type == HEAP_TINY) {

		arena_fastbin_drain(to_free);
		if (g_arena.tiny_cache == NULL) {
			g_arena.tiny_cache = to_free;
			return (F_NO_ERROR);
		}
	}
	else if (type == HEAP_SMALL) {

		arena_smallbin_drain(to_free);
		if (g_arena.small_cache == NULL) {
			g_arena.small_cache = to_free;
			return (F_NO_ERROR);
		}
	}
	return (arena_heap_munmap(to_free));

}

void		arena_bin_set(t_heap *heap, t_chunk *chunk, t_heap_type type) {

	if (type == HEAP_TINY) {

        arena_fastbin_set(heap, chunk);
		heap_update_alloc_chunks(heap, -1);
	}
    else if (type == HEAP_SMALL) {

        arena_smallbin_set(heap, chunk);
		heap_update_alloc_chunks(heap, -1);
	}
	else if (type == HEAP_LARGE) {

		heap_update_alloc_chunks(heap, -1);
	}
}

void		*arena_get_new_chunk_type(void *ptr, size_t p_new_size, size_t cur_size) {

	void *new_ptr = malloc_internal(p_new_size);
	if (!new_ptr) {
		return (NULL);
	}
	move_data(new_ptr, ptr, (p_new_size <= cur_size) ? p_new_size : cur_size);
	free_internal(data_to_chunk(ptr));
	return (new_ptr);
}

t_heap		*arena_find_cached_heap(size_t zone_size) {

	t_heap *cached_heap = NULL;

	if (zone_size == TINY_HEAP_SIZE && g_arena.tiny_cache != NULL) {

		cached_heap = g_arena.tiny_cache;
		g_arena.tiny_cache = NULL;
	}
	else if (zone_size == SMALL_HEAP_SIZE && g_arena.small_cache != NULL) {

		cached_heap = g_arena.small_cache;
		g_arena.small_cache = NULL;
	}
	return (cached_heap);
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
		do_perturb((char*)chunk + CHUNK_INUSE_SIZE, get_perturb_alloc(), get_size(chunk));

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


	if (has_perturb())
		do_perturb((char*)freed_chunk + CHUNK_FREE_SIZE, get_perturb_free(), get_size(freed_chunk) - 16);

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
			do_perturb((char*)head + CHUNK_INUSE_SIZE, get_perturb_alloc(), get_size(head));
		return (head);
	}

	tail = head->prev;
	tail->prev->next = head;
	head->prev = tail->prev;
	tail->next = NULL;
	tail->prev = NULL;
	set_flags(tail, IN_USE);

	if (has_perturb())
		do_perturb((char*)tail + CHUNK_INUSE_SIZE, get_perturb_alloc(), get_size(tail));
	return (tail);
}

void		arena_smallbin_set(t_heap *heap, t_chunk *freed_chunk) {

	size_t		size;
	int			index;
	t_chunk		*head = NULL;
	t_chunk		*tail = NULL;
	t_chunk		*next = NULL;


	if (!heap || !freed_chunk)
		return ;

	if ((freed_chunk = chunk_coalesce(heap, freed_chunk)) == NULL)
		return ;
		
	
	size = get_size(freed_chunk);
	if ((index = SBIN_IDX(size)) == -1)
		return ;
	unset_flags(freed_chunk, IN_USE);
	next = get_next_chunk(heap, freed_chunk);
	if (next)
		set_prevsize(next, size);
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
	

	if (has_perturb()) {
		do_perturb((char*)freed_chunk + CHUNK_FREE_SIZE, get_perturb_free(), get_size(freed_chunk) - 16);
	}

}

void		arena_smallbin_unlink(t_chunk *chunk) {

    if (!chunk) return;


    size_t  size  = get_size(chunk);
    int     index = SBIN_IDX(size);

    if (index == -1) {
		
		return;
	}
    if (chunk->next == chunk) {

        g_arena.smallbin[index] = NULL;
        chunk->next = NULL;
        chunk->prev = NULL;
        return;
    }
    chunk->prev->next = chunk->next;
    chunk->next->prev = chunk->prev;
    if (g_arena.smallbin[index] == chunk)
        g_arena.smallbin[index] = chunk->next;
    chunk->next = NULL;
    chunk->prev = NULL;
}

void		arena_smallbin_drain(t_heap *heap) {

	char		*cur = (char*)heap_to_chunk(heap);
	char		*end = cur + heap->total_size - heap_free_size(heap);

	while (cur < end) {

		
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


	if (zone_size == TINY_HEAP_SIZE) {

		return (&g_arena.tiny);
	}
	else if (zone_size == SMALL_HEAP_SIZE) {

		return (&g_arena.small);
	}
	else {
		
		return (&g_arena.large);
	}
	return (NULL);
}

int     	size_exceeds_rlimit(size_t aligned_size) {

    struct rlimit   rl;

    if (getrlimit(RLIMIT_AS, &rl) == -1)
        return (0);

    if (rl.rlim_cur == RLIM_INFINITY)
        return (aligned_size > USERSPACE_MAX);

    return ((rlim_t)aligned_size > rl.rlim_cur);
}

void		update_arena_heap_count(int count) {

	if (count == -1)
		g_arena.heap_count  = (g_arena.heap_count >= 1) ? g_arena.heap_count - 1 : 0;
	else if (count == 1)
		g_arena.heap_count += 1;
}

void    	arena_error_exit(int err) {

    char *msg;

    switch (err)
    {
        case F_MUMMAP_ERROR:
            msg = F_MUNMAP_MSG;
            break;
        case F_NO_ERROR:
            return ;
        case F_INV_PTR_ERROR:
            msg = F_INV_PTR_MSG;
            break;
        case F_DOUBLE_FREE_ERROR:
            msg = F_DOUBLE_FREE_MSG;
            break;
		case R_INV_PTR_ERROR:
            msg = R_INV_PTR_MSG;
            break;
        default:
            msg = M_UNKNOWN_MSG;
            break;
    }
    uint8_t check = get_check();
    
    if (check == M_CHECK_PRINT || check == M_CHECK_DEFAULT) {

        write(STDERR_FILENO, msg, M_ERR_MSG_SIZE);
    }
    if (check == M_CHECK_ABORT || check == M_CHECK_DEFAULT) {

        pthread_mutex_unlock(&g_arena.lock);
        abort();
    }
}
