#include "../../include/malloc.h"

t_chunk		*chunk_new(char *start, size_t size, size_t prev_s, size_t next_s, size_t flags) {

	t_chunk *new_chunk = (t_chunk*)start;

	ft_memset(new_chunk, 0, CHUNK_INUSE_SIZE);


	set_flags(new_chunk, flags);
	set_size(new_chunk, size);
	set_prevsize(new_chunk, prev_s);
	set_nextsize(new_chunk, next_s);
	new_chunk->next = NULL;
	new_chunk->prev = NULL;
	return (new_chunk);
}

void		chunk_split_center(t_heap *heap, t_chunk *chunk, size_t need) {

	t_chunk	*next         = get_next_chunk(heap, chunk);
	t_chunk	*prev         = get_prev_chunk(heap, chunk);

	set_size(chunk, need);
	if (prev)
		set_nextsize(prev, need);

	size_t	new_free_size = get_size(chunk) - CHUNK_INUSE_SIZE - need;
	t_chunk *new_free     = chunk_new( (char*)chunk + CHUNK_INUSE_SIZE + need, new_free_size, need, 0, IN_USE);

	if (next) {
		set_nextsize(new_free, get_size(next));
		set_prevsize(next, get_size(new_free));
	}
	arena_fastbin_set(heap, new_free);
}

void		chunk_split_right(t_heap *heap, t_chunk *chunk, t_chunk *next, size_t need) {

	size_t	new_size   = get_size(chunk) + need;
	size_t	next_total = get_size(next) + CHUNK_INUSE_SIZE;
	t_chunk *new_free  = (t_chunk*)((char*)chunk + CHUNK_INUSE_SIZE + new_size);
	t_chunk	*nnc       = get_next_chunk(heap, next);
	t_chunk *prev 	   = get_prev_chunk(heap, chunk);
	

	if (prev) set_nextsize(prev, new_size);

	set_size(chunk, new_size);

	if (has_flags(next, IS_CIS)) {

		t_chunk *new_free = heap_split_cis_mem(heap, need);
		if (has_perturb()) ft_memset(new_free, get_perturb_alloc(), get_size(new_free) + CHUNK_INUSE_SIZE);
		new_free = NULL;
		return ;
	}

	arena_fastbin_unlink(next);
	if (has_perturb()) ft_memset(chunk_to_data(chunk) + get_size(chunk), get_perturb_alloc(), need);
	
	if (next_total > new_size) {

		set_size(new_free, next_total - need - CHUNK_INUSE_SIZE);
		set_prevsize(new_free, new_size);
		if (has_perturb()) ft_memset(chunk_to_data(new_free) , get_perturb_free(), get_size(new_free));
		chunk_relink(chunk, new_free, nnc);
		arena_fastbin_set(heap, new_free);
		return ;
	}
	chunk_relink(chunk, new_free, nnc);
}

void		chunk_split_left(t_heap *heap, t_chunk *chunk, t_chunk *prev, size_t need) {

	size_t	prev_total  = get_size(prev) + CHUNK_INUSE_SIZE;
	size_t	new_size   = get_size(chunk) + need;
	t_chunk	*nnc       = get_next_chunk(heap, chunk);
	t_chunk *new_free  = NULL;
	
	
	ft_memmove(chunk_to_data(prev), chunk_to_data(chunk), get_size(chunk));
	set_size(prev, new_size);
	set_flags(prev, IN_USE);

	if (prev_total > need) {
		
        new_free = (t_chunk*)((char*)prev + CHUNK_INUSE_SIZE + new_size);
        set_size(new_free, prev_total - need - CHUNK_INUSE_SIZE);
        set_prevsize(new_free, new_size);
        unset_flags(new_free, IN_USE);
		if (has_perturb()) {
			ft_memset(chunk_to_data(new_free) , get_perturb_free(), get_size(new_free));
		}
	}

	chunk_relink(prev, new_free, nnc);
}

void		chunk_relink(t_chunk *prev, t_chunk *center, t_chunk *next) {

    if (center) {

        set_nextsize(prev, get_size(center));
        if (next) {

            set_nextsize(center, get_size(next));
            set_prevsize(next, get_size(center));
        }
    }
	else if (next) {

        set_nextsize(prev, get_size(next));
        set_prevsize(next, get_size(prev));
    }
}

bool		chunk_covers_entire_heap(t_heap *heap, t_chunk *chunk) {

	return (heap->total_size == (get_size(chunk) + CHUNK_INUSE_SIZE + heap_cis_mem_size(heap)));
}

bool		chunk_belongs_to_heap(t_heap *heap, t_chunk *chunk) {

	if (!heap || !chunk)
		return (false);
	char *start = (char*)(heap + 1);
	char *end = start + heap->total_size;

	return ((char *)chunk >= start && (char *)chunk <= end);
}

t_chunk		*get_next_chunk(t_heap *heap, t_chunk *chunk) {

	char *addr = ((char*)chunk + CHUNK_INUSE_SIZE + get_size(chunk));

	if (addr >= (char*)heap + sizeof(t_heap) + heap->total_size)
		return (NULL);
	return ( (t_chunk *)((char*)chunk + CHUNK_INUSE_SIZE + get_size(chunk)) );
}

t_chunk		*get_prev_chunk(t_heap *heap, t_chunk *chunk) {

	char *addr = ((char*)chunk - get_prevsize(chunk) - CHUNK_INUSE_SIZE);

	if (addr < (char*)heap + sizeof(t_heap))
		return (NULL);
	return ( (t_chunk *)(addr) );
}

bool		next_chunk_suffices(t_chunk *next, size_t need) {

	size_t next_size = get_size(next) + CHUNK_INUSE_SIZE;

	if (!has_flags(next, IN_USE) && next_size >= need && next_size >= MIN_TRIM) {

		return (true);
	}

	return (false);
}

bool		prev_chunk_suffices(t_chunk *prev, size_t need) {

	size_t prev_total = get_size(prev) + CHUNK_INUSE_SIZE;

	if (!has_flags(prev, IN_USE) && prev_total >= need && prev_total >= MIN_TRIM) {

		return (true);
	}

	return (false);
}

void		*chunk_to_data(t_chunk *chunk_addr) {

	return ((void *)((char*)chunk_addr + CHUNK_INUSE_SIZE));
}

t_chunk    *data_to_chunk(void *data_addr) {

	return ((t_chunk *)((char*)data_addr - CHUNK_INUSE_SIZE));
}

void		chunk_perturb(t_chunk *chunk, int FLAGS) {

	if (!chunk) return ;

	void *data = chunk_to_data(chunk);
	if ((char *)data == NULL) return ;

	int perturb_type;
	
	if (FLAGS == FREE_PERTURB)
		perturb_type = get_perturb_free();
	else if (FLAGS == ALLOC_PERTURB)
		perturb_type = get_perturb_alloc();
	else
		return ;
	
	ft_memset(data, perturb_type, get_size(chunk));
}
