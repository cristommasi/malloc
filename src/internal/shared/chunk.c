#include "../../../include/malloc_internal.h"

t_chunk		*chunk_new(char *start, size_t prev_s, size_t size, size_t flags) {

	t_chunk *new_chunk = (t_chunk*)start;

	do_perturb(new_chunk, 0, CHUNK_FREE_SIZE);
	
	set_prevsize(new_chunk, prev_s);
	set_size(new_chunk, size);
	set_flags(new_chunk, flags);

	return (new_chunk);
}

t_chunk		*chunk_realloc_in_place(t_heap *heap, t_chunk *chunk, size_t size) {

	t_chunk *next 	  = get_next_chunk(heap, chunk);
	t_chunk *prev     = get_prev_chunk(heap, chunk);
  	size_t   cur_size = get_size(chunk);
    size_t   need     = size - cur_size;


	if (size < cur_size  && size >= SMALL_CHUNK_MIN) {
		
        return (chunk_split_center(heap, chunk, cur_size, size));
    }
	else if (next && next_chunk_suffices(next, need) && heap->free_cis_start && heap->free_cis_start == next) {

		return (chunk_split_cis(heap, chunk, need, size));
	}
	else if (next && next_chunk_suffices(next, need)) {

		return (chunk_split_right(heap, chunk, next, need));
	}
	else if (prev && prev_chunk_suffices(prev, need)) {

		return (chunk_split_left(heap, chunk, prev, need));
	}
	return (NULL);
}

t_chunk		*chunk_split_center(t_heap *heap, t_chunk *chunk, size_t old_size, size_t new_size) {
	
	
	size_t	free_size = old_size - new_size - CHUNK_INUSE_SIZE;

	set_size(chunk, new_size);

	t_chunk	*new_free = chunk_new(
		(char *)chunk + CHUNK_INUSE_SIZE + new_size,
		0,
		free_size,
		IN_USE
	);

	arena_smallbin_set(heap, new_free);
	return (chunk);
}

t_chunk		*chunk_split_right(t_heap *heap, t_chunk *chunk, t_chunk *next, size_t need) {

	size_t	new_size   = get_size(chunk) + need;
	size_t	next_total = get_size(next) + CHUNK_INUSE_SIZE;
	t_chunk	*nnc       = NULL;
	size_t leftover    = next_total - need;
	

	arena_smallbin_unlink(next);
	set_size(chunk, new_size);
	set_flags(chunk, IN_USE);
	
	if (leftover >= CHUNK_INUSE_SIZE + MIN_TRIM){

		t_chunk *new_free = chunk_new(
        	(char*)chunk + CHUNK_INUSE_SIZE + new_size,
        	0,
        	leftover - CHUNK_INUSE_SIZE,
        	NO_FLAGS
   	 	);

		if (has_perturb())
			do_perturb((char*)new_free + CHUNK_FREE_SIZE , get_perturb_free(), get_size(new_free) - 16);

		if ((nnc = get_next_chunk(heap, next)) != NULL)
			set_prevsize(nnc, get_size(new_free));
		
		arena_smallbin_set(heap, new_free);
		return (chunk);
	}
	if ((nnc = get_next_chunk(heap, next)) != NULL)
		set_prevsize(nnc, 0);
	return (chunk);
}

t_chunk		*chunk_split_left(t_heap *heap, t_chunk *chunk, t_chunk *prev, size_t need) {

	size_t	prev_total  = get_size(prev) + CHUNK_INUSE_SIZE;
	size_t	new_size    = get_size(chunk) + need;
	t_chunk	*nnc        = NULL;
	

	arena_smallbin_unlink(prev);
	t_chunk	*new_inuse_chunk = chunk_new((char*)prev, get_prevsize(prev), new_size, IN_USE);
	move_data(chunk_to_data(prev), chunk_to_data(chunk), get_size(chunk));

	if (prev_total > need) {
		
		size_t	new_free_size = prev_total - need - CHUNK_INUSE_SIZE;
		t_chunk *new_free = chunk_new(
			(char*)prev + CHUNK_INUSE_SIZE + new_size,
			0,
			new_free_size,
			NO_FLAGS
		);

		if (has_perturb())
			do_perturb((char*)new_free + CHUNK_FREE_SIZE, get_perturb_free(), new_free_size - 16);
		
		if ((nnc = get_next_chunk(heap, new_free)) != NULL)
			set_prevsize(nnc, new_free_size);
		
		arena_smallbin_set(heap, new_free);
		return (new_inuse_chunk);
	}
	if ((nnc = get_next_chunk(heap, chunk)) != NULL)
		set_prevsize(nnc, 0);
	return (new_inuse_chunk);
}

t_chunk		*chunk_split_cis(t_heap *heap, t_chunk *chunk, size_t need, size_t size) {

	t_chunk *new_inuse_split = heap_split_cis_mem(heap, need);
	do_perturb((char*)new_inuse_split, 0, need + CHUNK_INUSE_SIZE);

	if (has_perturb())
		do_perturb((char*)new_inuse_split, get_perturb_alloc(), need + CHUNK_INUSE_SIZE);

	set_size(chunk, size);
	return (chunk);
}

t_chunk		*chunk_coalesce(t_heap *heap, t_chunk *freed_chunk) {

	if (!heap || !freed_chunk) return (NULL);

	t_chunk *next		 = get_next_chunk(heap, freed_chunk);
	t_chunk *prev		 = get_prev_chunk(heap, freed_chunk);
	size_t prev_size     = 0;
	size_t new_size      = 0;


	if (next != NULL && heap->free_cis_start && next == heap->free_cis_start) {


		prev_size = get_prevsize(freed_chunk);
		new_size = get_size(freed_chunk) + get_size(heap->free_cis_start) + CHUNK_INUSE_SIZE;

		do_perturb((char*)heap->free_cis_start, 0, CHUNK_FREE_SIZE);
		heap->free_cis_start = freed_chunk;

		unset_flags(heap->free_cis_start, IN_USE);
		set_size(heap->free_cis_start, new_size);
		set_prevsize(heap->free_cis_start, prev_size);
		set_flags(heap->free_cis_start, IS_CIS);

		if (has_perturb())
			do_perturb((char*)heap->free_cis_start + CHUNK_FREE_SIZE, get_perturb_free(), new_size - 16);
		return (NULL);
	}
	if (prev != NULL && !has_flags(prev, IS_CIS) && !has_flags(prev, IN_USE)) {
	
		prev_size = get_prevsize(prev);
		new_size = get_size(freed_chunk) + get_size(prev) + CHUNK_INUSE_SIZE;
		if (new_size <= SMALL_CHUNK_MAX) {
		
			arena_smallbin_unlink(prev);
			set_size(prev, new_size);
			set_prevsize(prev, prev_size);
			return (prev);
		}
	}
	if (next != NULL && !has_flags(next, IS_CIS) && !has_flags(next, IN_USE)) {

		prev_size = get_prevsize(freed_chunk);
		new_size = get_size(freed_chunk) + get_size(next) + CHUNK_INUSE_SIZE;
		if (new_size <= SMALL_CHUNK_MAX) {
			
			arena_smallbin_unlink(next);
			set_size(freed_chunk, new_size);
			set_prevsize(freed_chunk, prev_size);
			return (freed_chunk);
		}
	}
	return (freed_chunk);
}

bool		chunk_belongs_to_heap(t_heap *heap, t_chunk *chunk) {

	if (!heap || !chunk)
		return (false);
	char *start = (char*)heap + sizeof(t_heap);
	char *end = start + heap->total_size;

	return ((char *)chunk >= start && (char *)chunk <= end);
}

t_chunk		*get_next_chunk(t_heap *heap, t_chunk *chunk) {

	if (!chunk) return (NULL);
	char *addr = ((char*)chunk + CHUNK_INUSE_SIZE + get_size(chunk));
	char *end = (char*)heap_to_chunk(heap) + heap->total_size;

	if (addr >= end) {

		return (NULL);
	}
	return ( (t_chunk *)(addr) );
}

t_chunk		*get_prev_chunk(t_heap *heap, t_chunk *chunk) {

	if (!chunk) return (NULL);
	size_t prev_size = get_prevsize(chunk);
	if (!prev_size)
		return (NULL);
	char *addr = ((char*)chunk - prev_size - CHUNK_INUSE_SIZE);

	if (addr < (char*)heap + sizeof(t_heap))
		return (NULL);
	return ( (t_chunk *)(addr) );
}

bool		next_chunk_suffices(t_chunk *next, size_t need) {

	size_t next_size = get_size(next) + CHUNK_INUSE_SIZE;

	if (!has_flags(next, IN_USE) && next_size >= need && next_size >= SMALL_CHUNK_MIN) {

		return (true);
	}
	return (false);
}

bool		prev_chunk_suffices(t_chunk *prev, size_t need) {

	size_t prev_total = get_size(prev) + CHUNK_INUSE_SIZE;

	if (!has_flags(prev, IN_USE) && prev_total >= need && prev_total >= SMALL_CHUNK_MIN) {

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


