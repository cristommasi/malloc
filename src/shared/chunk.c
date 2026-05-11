#include "../../include/malloc.h"

t_chunk		*chunk_new(char *start, size_t prev_s, size_t size, size_t flags) {

	t_chunk *new_chunk = (t_chunk*)start;

	ft_memset(new_chunk, 0, CHUNK_INUSE_SIZE);


	set_prevsize(new_chunk, prev_s);
	set_size(new_chunk, size);
	set_flags(new_chunk, flags);
	return (new_chunk);
}


t_chunk		*chunk_realloc_in_place(t_heap *heap, t_chunk *chunk, size_t size) {

	t_chunk *next 	  = get_next_chunk(heap, chunk);
	t_chunk *prev     = get_prev_chunk(heap, chunk);
  	size_t   cur_size = get_size(chunk);
    size_t   need     = cur_size - size;



	if (size < cur_size && size >= MIN_TRIM) {
		
        chunk_split_center(heap, chunk, need);
        return (chunk);
    }
	else if (next && next_chunk_suffices(next, need)) {

		chunk_split_right(heap, chunk, next, need);
		return (chunk);
	}
	else if (prev && prev_chunk_suffices(prev, need)) {

		arena_smallbin_unlink(prev);
		chunk_split_left(heap, chunk, prev, need);
		return (prev);
	}
	return (NULL);
}

void	chunk_split_center(t_heap *heap, t_chunk *chunk, size_t new_size) {
	
	t_chunk	*next;
	t_chunk	*new_free;
	size_t	old_size;
	size_t	free_size;

	old_size = get_size(chunk);

	if (old_size <= new_size + CHUNK_INUSE_SIZE)
		return ;

	free_size = old_size - new_size - CHUNK_INUSE_SIZE;

	set_size(chunk, new_size);



	new_free = chunk_new(
		(char *)chunk + CHUNK_INUSE_SIZE + new_size,
		0,
		free_size,
		NO_FLAGS
	);
	heap->blocks += 1;
	next = get_next_chunk(heap, new_free);

	if (next)
		set_prevsize(next, free_size);

	arena_smallbin_set(heap, new_free);
}

void		chunk_split_right(t_heap *heap, t_chunk *chunk, t_chunk *next, size_t need) {

	size_t	new_size   = get_size(chunk) + need;
	size_t	next_total = get_size(next) + CHUNK_INUSE_SIZE;
	t_chunk *new_free  = (t_chunk*)((char*)chunk + CHUNK_INUSE_SIZE + new_size);
	t_chunk	*nnc       = get_next_chunk(heap, next);

	

	set_size(chunk, new_size);

	if (has_flags(next, IS_CIS)) {

		t_chunk *new_free = heap_split_cis_mem(heap, need);
		if (has_perturb()) ft_memset((char*)new_free, get_perturb_alloc(), get_size(new_free) + CHUNK_INUSE_SIZE);
		new_free = NULL;
		return ;
	}

	arena_smallbin_unlink(next);
	if (has_perturb()) ft_memset((char*)chunk_to_data(chunk) + get_size(chunk), get_perturb_alloc(), need);
	
	if (next_total > new_size) {

		set_size(new_free, next_total - need - CHUNK_INUSE_SIZE);
		set_prevsize(new_free, new_size);
		if (has_perturb()) ft_memset((char*)new_free + CHUNK_FREE_SIZE , get_perturb_free(), get_size(new_free) - 16);
		chunk_relink(chunk, new_free, nnc);
		arena_smallbin_set(heap, new_free);
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

        if (next) {

            set_prevsize(next, get_size(center));
        }
    }
	else if (next) {

        set_prevsize(next, get_size(prev));
    }
}

int		chunk_coalesce(t_heap *heap, t_chunk *freed_chunk) {

	t_chunk *next = get_next_chunk(heap, freed_chunk);
	t_chunk *prev = get_prev_chunk(heap, freed_chunk);


	if (prev != NULL) {

		size_t prev_size = get_size(prev);
		if (!has_flags(prev, IS_CIS) && !has_flags(next, IN_USE)) {

			size_t new_size = get_size(freed_chunk) + CHUNK_INUSE_SIZE + prev_size;
			if (prev_size  <= SMALL_CHUNK_MAX) {
				arena_smallbin_unlink(prev);
				set_size(prev, new_size);
				freed_chunk = prev;
			}
		}
	}
	if (next != NULL) {

		size_t next_size = get_size(next);
		size_t new_size = get_size(freed_chunk) + CHUNK_INUSE_SIZE + next_size;

		if (!has_flags(next, IS_CIS) && !has_flags(next, IN_USE)) {

			if (new_size  <= SMALL_CHUNK_MAX) {
				arena_smallbin_unlink(next);
				set_size(freed_chunk, new_size);
			}
		}
		else if (has_flags(next, IS_CIS)) {

			set_size(freed_chunk, new_size);
			heap->free_cis_start = freed_chunk;
			heap->blocks = (heap->blocks >= 1) ? heap->blocks - 1 : 0;
			return (1);
		}
		else if (has_flags(next, IN_USE)) {
			set_prevsize(next,  get_size(freed_chunk));
		}
	}
	return (0);
}

bool		chunk_covers_entire_heap(t_heap *heap, t_chunk *chunk) {

	return (heap->total_size == (get_size(chunk) + CHUNK_INUSE_SIZE + heap_cis_mem_size(heap)));
}

bool		chunk_belongs_to_heap(t_heap *heap, t_chunk *chunk) {

	if (!heap || !chunk)
		return (false);
	char *start = (char*)heap + sizeof(t_heap);
	char *end = start + heap->total_size;

	return ((char *)chunk >= start && (char *)chunk <= end);
}

t_chunk		*get_next_chunk(t_heap *heap, t_chunk *chunk) {


	char *addr = ((char*)chunk + CHUNK_INUSE_SIZE + get_size(chunk));
	char *end = (char*)heap_to_chunk(heap) + heap->total_size;

	if (addr >= end) {

		return (NULL);
	}
	return ( (t_chunk *)((char*)chunk + CHUNK_INUSE_SIZE + get_size(chunk)) );
}

t_chunk		*get_prev_chunk(t_heap *heap, t_chunk *chunk) {

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
	size_t size = get_size(chunk);
	if (!size) return ;


	if (FLAGS == FREE_PERTURB) {
		if (size < 16) return ;
		void *data = (char*)chunk + CHUNK_FREE_SIZE;
        ft_memset(data, get_perturb_free(), size - CHUNK_INUSE_SIZE);
	}
    else if (FLAGS == ALLOC_PERTURB) {

        void *data = (char*)chunk + CHUNK_INUSE_SIZE;
        ft_memset(data, get_perturb_alloc(), size);
    }

}
