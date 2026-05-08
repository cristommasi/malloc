#include "../../include/malloc.h"


t_heap		*heap_new_and_append(size_t size) {

	size_t  zone_size = heap_page_size(size);
	t_heap *new_heap  = heap_new(zone_size);
	
	if (new_heap == MAP_FAILED) {
		return (MAP_FAILED);
	}
	if (size >= get_mmap_threshold()) {

		heap_append(&g_arena.large, new_heap);
	}
	else if (zone_size == TINY_HEAP_SIZE) {

		heap_append(&g_arena.tiny, new_heap);
	}
	else if (zone_size == SMALL_HEAP_SIZE) {

		heap_append(&g_arena.small, new_heap);
	}
	return (new_heap);
}

t_heap		*heap_new(size_t zone_size) {

	if (has_arena_max() && g_arena.count + 1 >= get_arena_max()) {
		return (MAP_FAILED);
	}
	t_heap  *new_heap = (t_heap *)mmap(NULL, zone_size + sizeof(t_heap), PROT_FLAGS, MAP_FLAGS, NO_FD, NO_OFFSET);
	
	if (new_heap == MAP_FAILED) {
		return (MAP_FAILED);
	}
	g_arena.count += 1;

	new_heap->blocks         = 0;
	new_heap->total_size     = zone_size;
	new_heap->free_cis_start = heap_to_chunk(new_heap);
	new_heap->next           = NULL;
	new_heap->prev           = NULL;

	size_t flags = (zone_size != TINY_HEAP_SIZE && zone_size != SMALL_HEAP_SIZE) ? (IS_LARGE | IS_CIS) : IS_CIS;

	t_chunk *new_chunk = chunk_new((char*)new_heap->free_cis_start, 0, zone_size - CHUNK_INUSE_SIZE, flags);

	if (has_perturb())
		chunk_perturb(new_chunk, FREE_PERTURB);
	return (new_heap);
}

void		heap_append(t_heap **HEAP_TYPE, t_heap *new_heap) {

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
	new_heap->prev = prev;
	prev->next = new_heap;
}

t_chunk		*heap_split_cis_mem(t_heap *heap, size_t size) {

    t_chunk *new_inuse_chunk = heap->free_cis_start;
	size_t	remaining		 = heap_free_size(heap);


	unset_flags(new_inuse_chunk, IS_CIS);
	set_size(new_inuse_chunk, size);
	set_flags(new_inuse_chunk, IN_USE);
	new_inuse_chunk->next = NULL;
	new_inuse_chunk->prev = NULL;
	if (has_perturb())
		chunk_perturb(new_inuse_chunk, ALLOC_PERTURB);

	if (remaining > size + CHUNK_INUSE_SIZE + CHUNK_FREE_SIZE) {

		size_t	new_free_size = remaining - size - CHUNK_INUSE_SIZE - CHUNK_FREE_SIZE;

		t_chunk *new_free_chunk = chunk_new( 
			(char *)new_inuse_chunk + CHUNK_INUSE_SIZE + size, 
			0,
			new_free_size, 
			IS_CIS
		);

		heap->free_cis_start = new_free_chunk;
	}
	else {

		heap->free_cis_start = NULL;
	}
	return (new_inuse_chunk);
}

t_chunk		*heap_find_cis_mem_chunk(size_t size) {

	t_heap  **cur = arena_heap_group_by_chunk(size);
	t_chunk *chunk = NULL;


	while (*cur != NULL) {
		
		if (heap_free_size(*cur) >= size + CHUNK_INUSE_SIZE) {
			if ((chunk = heap_split_cis_mem(*cur, size)) == NULL)
				return (NULL);
			(*cur)->blocks += 1;
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

bool		heap_has_remaining_cis(t_heap *heap, size_t size) {

	if (heap->free_cis_start == NULL)
		return (false);
	return (((char*)heap->free_cis_start + size + CHUNK_INUSE_SIZE) <= (char *)heap_to_chunk(heap) + heap->total_size);
}

size_t		heap_cis_mem_size(t_heap *heap) {

	if (heap->free_cis_start == NULL)
		return (0);
	char *end = (char *)(heap + 1) + heap->total_size;

	return ((size_t)(end - (char *)heap->free_cis_start));
}

bool		heap_cis_mem_fits_chunk(t_heap *heap, size_t to_add) {

    return (heap_cis_mem_size(heap) >= to_add);
}

t_chunk *heap_to_chunk(t_heap *heap) {

	uintptr_t	addr = (uintptr_t)heap + sizeof(t_heap);
	addr = ALIGN(addr);

	return ((t_chunk *)addr);
}

bool		heap_is_different_type(size_t sizeA, size_t sizeB) {

	return (heap_type(sizeA) != heap_type(sizeB));

}

size_t		heap_page_size(size_t size) {

	if (size >= get_mmap_threshold()) {
		return (size + CHUNK_INUSE_SIZE);
	}
	else if (size <= TINY_CHUNK_MAX) {
		return (TINY_HEAP_SIZE);
	}
	else if (size <= SMALL_CHUNK_MAX) {
		return (SMALL_HEAP_SIZE); 
	}
	return (0);
}

t_heap_type		heap_type(size_t size)
{
	size = ALIGN(size);

	if (size <= TINY_CHUNK_MAX)
		return (HEAP_TINY);

	if (size <= SMALL_CHUNK_MAX)
		return (HEAP_SMALL);

	return (HEAP_LARGE);
}

size_t		heap_chunk_size(size_t size) {

	size = ALIGN(size);

	if (get_mmap_threshold() >= size) {
		return (size);
	}
	else if (size <= TINY_CHUNK_MAX) {
		return (TINY_CHUNK_MAX);
	}
	else if (size <= SMALL_CHUNK_MAX) {
		return (SMALL_CHUNK_MAX); 
	}
	return (0);
}