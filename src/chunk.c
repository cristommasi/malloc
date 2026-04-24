#include "../include/malloc.h"

bool    chunk_covers_entire_heap(t_heap *heap, t_chunk *chunk) {

	return (heap->total_size == (get_size(chunk) + sizeof(t_chunk) + heap_cis_mem_size(heap)));
}


bool    chunk_belongs_to_heap(t_heap *heap, t_chunk *chunk) {

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

	return ((!has_flags(next, IN_USE) || has_flags(next, IS_CIS)) && next_size >= need && next_size >= MIN_TRIM);
}

bool prev_chunk_suffices(t_chunk *prev, size_t need) {

	size_t prev_total = get_size(prev) + sizeof(t_chunk);

	return (!has_flags(prev, IN_USE) && prev_total >= need && prev_total >= MIN_TRIM);
}

void chunk_relink(t_chunk *chunk, t_chunk *new_free, t_chunk *nnc) {

    if (new_free) {

        set_nextsize(chunk, get_size(new_free));
        if (nnc) {

            set_nextsize(new_free, get_size(nnc));
            set_prevsize(nnc, get_size(new_free));
        }
    }
	else if (nnc) {

        set_nextsize(chunk, get_size(nnc));
        set_prevsize(nnc, get_size(chunk));
    }
}

void    *chunk_to_data(t_chunk *chunk_addr) {

	return ((void *)(chunk_addr + sizeof(t_chunk)));
}


t_chunk    *data_to_chunk(void *data_addr) {

	return ((t_chunk *)((char*)data_addr - sizeof(t_chunk)));
}

