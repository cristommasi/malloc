#include "../../../include/malloc_internal.h"


t_heap		*heap_new_and_append(size_t size) {

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

t_heap		*heap_new(size_t zone_size) {

	if (has_arena_max() && g_arena.heap_count + 1 >= get_arena_max()) {
		return (MAP_FAILED);
	}
	t_heap  *new_heap = (t_heap *)mmap(NULL, zone_size + sizeof(t_heap), PROT_FLAGS, MAP_FLAGS, NO_FD, NO_OFFSET);
	
	if (new_heap == MAP_FAILED) {
		return (MAP_FAILED);
	}
	update_arena_heap_count(1);

	new_heap->alloc_chunks   = 0;
	new_heap->total_size     = zone_size;
	new_heap->free_cis_start = heap_to_chunk(new_heap);
	new_heap->next           = NULL;
	new_heap->prev           = NULL;

	size_t flags = (zone_size != TINY_HEAP_SIZE && zone_size != SMALL_HEAP_SIZE) ? (IS_LARGE | IS_CIS) : IS_CIS;

	t_chunk *new_chunk = chunk_new((char*)new_heap->free_cis_start, 0, zone_size - CHUNK_INUSE_SIZE, flags);
	
	if (has_perturb())
		do_perturb(((char*)new_chunk + CHUNK_FREE_SIZE), get_perturb_free(), get_size(new_chunk) - 16);
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

	size_t	remaining		 = heap_free_size(heap);


	t_chunk *new_inuse_chunk = chunk_new(
		(char*)heap->free_cis_start,
		0,
		size,
		IN_USE
	);
	if (has_perturb())
		do_perturb((char*)new_inuse_chunk + CHUNK_INUSE_SIZE, get_perturb_alloc(), size);


	if (remaining > size + CHUNK_INUSE_SIZE + CHUNK_FREE_SIZE) {

		size_t	new_free_size = remaining - size - CHUNK_INUSE_SIZE - CHUNK_FREE_SIZE;

		t_chunk *new_free_chunk = chunk_new( (char *)new_inuse_chunk + CHUNK_INUSE_SIZE + size, 0, new_free_size, IS_CIS);
		heap->free_cis_start = new_free_chunk;
		new_free_chunk->next = NULL;
		new_free_chunk->prev = NULL;
		if (has_perturb())
			do_perturb((char*)heap->free_cis_start + CHUNK_FREE_SIZE, get_perturb_free(), new_free_size - 16);
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
			(*cur)->alloc_chunks += 1;
			return (chunk);
		}
		cur = &(*cur)->next;
	}
	return (NULL);
}

size_t		heap_free_size(t_heap *heap) {

	if (!heap || !heap->free_cis_start)
		return (0);

	char *end = (char *)heap + sizeof(t_heap) + heap->total_size;
	if ((char *)heap->free_cis_start > end)
        return (0);
	return ((size_t)(end - (char *)heap->free_cis_start));
}

t_chunk 	*heap_to_chunk(t_heap *heap) {

	uintptr_t	addr = (uintptr_t)heap + sizeof(t_heap);

	return ((t_chunk *)addr);
}

bool		heap_is_different_type(size_t sizeA, size_t sizeB) {

	return (heap_type(sizeA) != heap_type(sizeB));

}

size_t		heap_page_size(size_t size) {

    if (size <= TINY_CHUNK_MAX)
        return (TINY_HEAP_SIZE);

    if (size <= SMALL_CHUNK_MAX)
        return (SMALL_HEAP_SIZE);
	
    return (size + CHUNK_INUSE_SIZE);
}

t_heap_type	heap_type(size_t size) {

	if (size <= TINY_CHUNK_MAX)
		return (HEAP_TINY);

	if (size <= SMALL_CHUNK_MAX)
		return (HEAP_SMALL);

	return (HEAP_LARGE);
}

void	heap_update_alloc_chunks(t_heap *heap, int block) {

	if (block == -1)
		heap->alloc_chunks = (heap->alloc_chunks >= 1) ? heap->alloc_chunks - 1 : 0;
	else if (block == 1)
		heap->alloc_chunks++;
}
	
	

