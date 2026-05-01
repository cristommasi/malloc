#include "../../include/malloc.h"


t_heap *heap_new_and_append(size_t size) {

	size_t  zone_size = heap_page_size(size);
	t_heap *new_heap  = heap_new(zone_size);
	
	if (new_heap == MAP_FAILED) {
		return (MAP_FAILED);
	}

	if (zone_size == TINY_HEAP_SIZE) {

		heap_append(&g_arena.tiny, new_heap);
	}
	else if (zone_size == SMALL_HEAP_SIZE) {

		heap_append(&g_arena.small, new_heap);
	}
	else {
		heap_append(&g_arena.large, new_heap);
	}
	return (new_heap);
}

t_heap    *heap_new(size_t zone_size) {

	t_heap  *new_heap = (t_heap *)mmap(NULL, zone_size + sizeof(t_heap), PROT_FLAGS, MAP_FLAGS, NO_FD, NO_OFFSET);
	
	if (new_heap == MAP_FAILED) {
		return (MAP_FAILED);
	}
	new_heap->blocks         = 0;
	new_heap->total_size     = zone_size;
	new_heap->free_cis_start = heap_to_chunk(new_heap);
	new_heap->next           = NULL;

    t_chunk *new_chunk = new_heap->free_cis_start;


	ft_memset(new_chunk, 0, sizeof(t_chunk));
	if (zone_size != TINY_HEAP_SIZE && zone_size != SMALL_HEAP_SIZE) {
		set_flags(new_chunk, IS_LARGE);
	}
	set_flags(new_chunk, IS_CIS);
	set_size(new_chunk, zone_size - sizeof(t_chunk));
	set_prevsize(new_chunk, 0);
	set_nextsize(new_chunk, 0);
	new_chunk->next = NULL;

	if (has_perturb()) {
		ft_memset(chunk_to_data(new_chunk) , get_perturb_free(), get_size(new_chunk));
	}
	return (new_heap);
}

void    heap_append(t_heap **HEAP_TYPE, t_heap *new_heap) {

	t_heap *cur = *HEAP_TYPE;
	t_heap *prev = NULL;

	if (*HEAP_TYPE == NULL) {
		*HEAP_TYPE = new_heap;
		return ;
	}
	while (cur != NULL) {

		prev = cur;
		cur = cur->next;
	}
	prev->next = new_heap;
}

t_chunk		*heap_split_cis_mem(t_heap *heap, size_t size) {

    t_chunk *new_inuse_chunk = heap->free_cis_start;
	size_t	remaining = heap_free_size(heap);


	ft_memset(new_inuse_chunk, 0, sizeof(t_chunk));
	set_size(new_inuse_chunk, size);
	set_flags(new_inuse_chunk, IN_USE);
	new_inuse_chunk->next = NULL;

	if (remaining > size + (2 * sizeof(t_chunk))) {

		t_chunk *new_free_chunk = (t_chunk *)((char *)new_inuse_chunk + sizeof(t_chunk) + size);
		size_t	free_size = remaining - size - (2 * sizeof(t_chunk));

		ft_memset(new_free_chunk, 0, sizeof(t_chunk));
		set_size(new_free_chunk, free_size);
		set_flags(new_free_chunk, IS_CIS);
		set_prevsize(new_free_chunk, size);
		new_free_chunk->next = NULL;
		heap->free_cis_start = new_free_chunk;
		set_nextsize(new_inuse_chunk, free_size);
	}
	else {

		heap->free_cis_start = NULL;
		set_nextsize(new_inuse_chunk, 0);
	}
	heap->blocks += 1;
	if (has_perturb()) {
		ft_memset(chunk_to_data(new_inuse_chunk) , get_perturb_alloc(), get_size(new_inuse_chunk));
	}
	return (new_inuse_chunk);
}

t_chunk  *heap_find_cis_mem_chunk(size_t size) {

	t_heap  **cur = arena_heap_group(size);
	t_chunk *chunk = NULL;


	while (*cur != NULL) {


		if (heap_free_size(*cur) >= size + sizeof(t_chunk)) {

			if ((chunk = heap_split_cis_mem(*cur, size)) == NULL)
				return (NULL);
			return (chunk);
		}
		cur = &(*cur)->next;
	}
	return (NULL);
}


t_chunk		*heap_realloc_in_place(t_heap *heap, t_chunk *chunk, size_t size) {

	t_chunk *next 	  = get_next_chunk(heap, chunk);
	t_chunk *prev     = get_prev_chunk(heap, chunk);
  	size_t   cur_size = get_size(chunk);
    size_t   need     = size - cur_size;



	if (size < cur_size && size >= MIN_TRIM) {

        chunk_split_left(heap, chunk, chunk, size);
        return (chunk);
    }
	else if ((next && next_chunk_suffices(next, need))) {


		chunk_split_right(heap, chunk, next, need);

		return (chunk);
	}
	else if (prev && prev_chunk_suffices(prev, need)) {

		arena_fastbin_unlink(prev);
		chunk_split_left(heap, chunk, prev, need);
		return (prev);
	}
	return (NULL);
}

bool		heap_has_remaining_cis(t_heap *heap, size_t size) {

	if (heap->free_cis_start == NULL)
		return (false);
	return (((char*)heap->free_cis_start + size + sizeof(t_chunk)) <= (char *)heap_to_chunk(heap) + heap->total_size);
}

size_t heap_cis_mem_size(t_heap *heap) {

	if (heap->free_cis_start == NULL)
		return (0);
	char *end = (char *)(heap + 1) + heap->total_size;

	return ((size_t)(end - (char *)heap->free_cis_start));
}

bool	heap_cis_mem_fits_chunk(t_heap *heap, size_t to_add) {

    return (heap_cis_mem_size(heap) >= to_add);
}


t_chunk *heap_to_chunk(t_heap *heap_addr) {

	if (!heap_addr) return (NULL);
    return ((t_chunk *)((char *)heap_addr + sizeof(t_heap)));
}


bool heap_is_different_type(size_t sizeA, size_t sizeB) {

	return (heap_chunk_size(sizeA) != heap_chunk_size(sizeB));

}

size_t  heap_page_size(size_t size) {

	if (size <= TINY_CHUNK_MAX) {
		return (TINY_HEAP_SIZE);
	}
	else if (size <= SMALL_CHUNK_MAX) {
		return (SMALL_HEAP_SIZE); 
	}
	else {
		return (size + sizeof(t_chunk));
	}
}

size_t		heap_type(size_t size) {

	if (size <= TINY_CHUNK_MAX) {
		return (TINY_CHUNK_MAX);
	}
	else if (size <= SMALL_CHUNK_MAX) {
		return (SMALL_CHUNK_MAX); 
	}
	else {
		return (LARGE_CHUNK_MIN);
	}

}

size_t	heap_chunk_size(size_t size) {

	size = ALIGN(size);

	if (size <= TINY_CHUNK_MAX) {
		return (TINY_CHUNK_MAX);
	}
	else if (size <= SMALL_CHUNK_MAX) {
		return (SMALL_CHUNK_MAX); 
	}
	else {
		return (size);
	}
}