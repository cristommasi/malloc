#include "../../include/malloc.h"


void chunk_split_right(t_heap *heap, t_chunk *chunk, t_chunk *next, size_t need) {

	size_t	new_size   = get_size(chunk) + need;
	size_t	next_total = get_size(next) + sizeof(t_chunk);
	t_chunk *new_free  = (t_chunk*)((char*)chunk + sizeof(t_chunk) + new_size);
	t_chunk	*nnc       = get_next_chunk(heap, next);
	

	if (!has_flags(next, IS_CIS))
		arena_fastbin_unlink(next);
	set_size(chunk, new_size);
	if (next_total > new_size) {

		set_size(new_free, next_total - need - sizeof(t_chunk));
		set_prevsize(new_free, new_size);
		if (has_flags(next, IS_CIS)) {
			set_flags(new_free, IS_CIS);
			heap->free_cis_start = new_free;
		}
		else if (!has_flags(next, IS_CIS)) {
			arena_fastbin_set(heap, new_free);
		}
	}
	else if (need == next_total && has_flags(next, IS_CIS)) {

		heap->free_cis_start = NULL;
	}
	chunk_relink(chunk, new_free, nnc);
}

void chunk_split_left(t_heap *heap, t_chunk *chunk, t_chunk *prev, size_t need) {

	size_t	prev_total  = get_size(prev) + sizeof(t_chunk);
	size_t	new_size   = get_size(chunk) + need;
	t_chunk	*nnc       = get_next_chunk(heap, chunk);
	t_chunk *new_free  = NULL;
	
	
	ft_memmove(chunk_to_data(prev), chunk_to_data(chunk), get_size(chunk));
	set_size(prev, new_size);
	set_flags(prev, IN_USE);

	if (prev_total > need) {
		
        new_free = (t_chunk*)((char*)prev + sizeof(t_chunk) + new_size);
        set_size(new_free, prev_total - need - sizeof(t_chunk));
        set_prevsize(new_free, new_size);
        unset_flags(new_free, IN_USE);
	}

	chunk_relink(prev, new_free, nnc);
}

void chunk_split_center(t_heap *heap, t_chunk *chunk, size_t need) {

	size_t	new_free_size = get_size(chunk) - sizeof(t_chunk) - need;
	t_chunk *new_free     = (t_chunk*)((char*)chunk + sizeof(t_chunk) + need);
	t_chunk	*next         = get_next_chunk(heap, chunk);
	t_chunk	*prev         = get_prev_chunk(heap, chunk);	
	
	set_size(chunk, need);
	set_size(new_free, new_free_size);
	arena_fastbin_set(heap, new_free);
	if (prev)
		set_nextsize(prev, need);
	chunk_relink(chunk, new_free, next);
}


void chunk_relink(t_chunk *prev, t_chunk *center, t_chunk *next) {

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

bool    chunk_covers_entire_heap(t_heap *heap, t_chunk *chunk) {

	return (heap->total_size == (get_size(chunk) + sizeof(t_chunk) + heap_cis_mem_size(heap)));
}


bool    chunk_belongs_to_heap(t_heap *heap, t_chunk *chunk) {

	if (!heap || !chunk)
		return (false);
	char *start = (char*)(heap + 1);
	char *end = start + heap->total_size;

	return ((char *)chunk >= start && (char *)chunk <= end);
}


t_chunk *get_next_chunk(t_heap *heap, t_chunk *chunk) {

	char *addr = ((char*)chunk + sizeof(t_chunk) + get_size(chunk));

	if (addr >= (char*)heap + sizeof(t_heap) + heap->total_size)
		return (NULL);
	return ( (t_chunk *)((char*)chunk + sizeof(t_chunk) + get_size(chunk)) );
}

t_chunk *get_prev_chunk(t_heap *heap, t_chunk *chunk) {

	char *addr = ((char*)chunk - get_prevsize(chunk) - sizeof(t_chunk));

	if (addr < (char*)heap + sizeof(t_heap))
		return (NULL);
	return ( (t_chunk *)(addr) );
}

bool next_chunk_suffices(t_chunk *next, size_t need) {

	size_t next_size = get_size(next) + sizeof(t_chunk);

	if (!has_flags(next, IN_USE) && next_size >= need && next_size >= MIN_TRIM) {

		return (true);
	}

	return (false);
}

bool prev_chunk_suffices(t_chunk *prev, size_t need) {

	size_t prev_total = get_size(prev) + sizeof(t_chunk);

	if (!has_flags(prev, IN_USE) && prev_total >= need && prev_total >= MIN_TRIM) {

		return (true);
	}

	return (false);
}



void    *chunk_to_data(t_chunk *chunk_addr) {

	return ((void *)((char*)chunk_addr + sizeof(t_chunk)));
}


t_chunk    *data_to_chunk(void *data_addr) {

	return ((t_chunk *)((char*)data_addr - sizeof(t_chunk)));
}

