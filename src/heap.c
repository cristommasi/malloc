#include "../include/malloc.h"


t_heap *heap_new_and_append(size_t size) {

	size_t  zone_size = heap_page_size(size);
	t_heap *new_heap  = heap_new(zone_size);
	
	if (new_heap == MAP_FAILED) {
		return (MAP_FAILED);
	}
	//printf("%s - Allocated new %s heap\n", __func__, ((heap_type(size) == TINY_HEAP) ? "TINY" : ((heap_type(size) == SMALL_HEAP) ? "SMALL" : "LARGE")));
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

		//printf("%s - mmap() FAILED\n", __func__);
		return (MAP_FAILED);
	}
	new_heap->total_size     = zone_size;
	new_heap->free_cis_start = heap_to_chunk(new_heap);
	new_heap->next           = NULL;
	new_heap->blocks         = 0;

    t_chunk *new_chunk = new_heap->free_cis_start;

	if (zone_size != TINY_HEAP_SIZE && zone_size != SMALL_HEAP_SIZE) {
		set_large_size(new_chunk, zone_size);
	}
	new_chunk->prev_size = 0;
    new_chunk->size = zone_size - sizeof(t_chunk);
    new_chunk->next = NULL;

	return (new_heap);
}

void    heap_append(t_heap **HEAP_TYPE, t_heap *new_heap) {

	t_heap *cur = *HEAP_TYPE;
	t_heap *prev = NULL;

	//printf("%s - Appended new heap\n", __func__);
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

t_heap  *heap_find_cis_mem(size_t size) {

	t_heap  **cur = arena_heap_group(size);


	while (*cur != NULL) {

		if (heap_has_remaining_cis(*cur, size)) {
			//printf("%s - %s heap with size req.\n", __func__, ((heap_type(size) == TINY_HEAP) ? "TINY" : "SMALL"));
			return (*cur);
		}
		cur = &(*cur)->next;
	}
	//printf("%s - found no heap with size large enough\n", __func__);
	return (NULL);

}

t_chunk		*heap_split_cis_mem(t_heap *heap, size_t size) {

    t_chunk *new_inuse_chunk = heap->free_cis_start;
    

	if (heap_is_large(size)) {

		set_large_size(new_inuse_chunk, heap_page_size(size));
		heap->blocks += 1;
		heap->free_cis_start = NULL;
		return (new_inuse_chunk);
	}
	new_inuse_chunk->prev_size = 0;
	new_inuse_chunk->size = (uint32_t)size;
    new_inuse_chunk->next = NULL;
	set_in_use(new_inuse_chunk);

	t_chunk *new_free_chunk = heap->free_cis_start + size + sizeof(t_chunk);
	new_free_chunk->prev_size = (uint32_t)size;
    new_free_chunk->size = new_inuse_chunk->size - size - (uint32_t)sizeof(t_chunk);
    new_free_chunk->next = NULL;
	set_prev_in_use(new_free_chunk);


	heap->blocks += 1;
	heap->free_cis_start = (t_chunk *)((char *)heap->free_cis_start + sizeof(t_chunk) + size);
	return (new_inuse_chunk);
}

int		heap_has_remaining_cis(t_heap *heap, size_t size) {

	return (((char*)heap->free_cis_start + size + sizeof(t_chunk)) <= (char *)heap_to_chunk(heap) + heap->total_size);
}

size_t heap_cis_mem_size(t_heap *heap) {

	char *end = (char *)(heap + 1) + heap->total_size;

	return ((size_t)(end - (char *)heap->free_cis_start));
}

bool	heap_cis_mem_fits_chunk(t_heap *heap, size_t to_add) {

    return (heap_cis_mem_size(heap) >= to_add);
}


t_chunk *heap_to_chunk(t_heap *heap_addr) {

    return ((t_chunk *)((char *)heap_addr + sizeof(t_heap)));
}

size_t  heap_page_size(size_t size) {

	if (size <= TINY_CHUNK_MAX) {
		return (TINY_HEAP_SIZE);
	}
	else if (size <= SMALL_CHUNK_MAX) {
		return (SMALL_HEAP_SIZE); 
	}
	else {
		return (size);
	}
}

t_heap_type  heap_type(size_t size) {

	if (size <= TINY_CHUNK_MAX) {
		return (TINY_HEAP);
	}
	else if (size <= SMALL_CHUNK_MAX) {
		return (SMALL_HEAP); 
	}
	else {
		return (LARGE_HEAP);
	}
}

size_t	heap_chunk_size(size_t size) {

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

bool	heap_is_large(size_t size) {

	size_t	zone_size = heap_page_size(size);

	return (zone_size != TINY_HEAP_SIZE && zone_size != SMALL_HEAP_SIZE);

}

bool heap_is_different_type(size_t sizeA, size_t sizeB) {

	return (heap_chunk_size(sizeA) != heap_chunk_size(sizeB));

}

t_chunk *	heap_split_chunk(t_heap *heap, t_chunk *chunk, size_t size) {

	t_chunk *new_free_chunk = (t_chunk*)((char*)chunk + sizeof(t_chunk) + size);

	new_free_chunk->size =  chunk->size - size - sizeof(t_chunk);
	new_free_chunk->prev_size = size;
	new_free_chunk->next = NULL;
	
	set_prev_in_use(new_free_chunk);
	arena_fastbin_set(heap, new_free_chunk);

	return (new_free_chunk);
}

t_chunk *heap_check_next_chunk(t_heap *heap, t_chunk *chunk, size_t new_size) {

	t_chunk *next_chunk = get_next_chunk(heap, chunk);

	if (!next_chunk)
		return (NULL);

	if (in_use(next_chunk))
		return (NULL);

	uint32_t next_total = (next_chunk->size + sizeof(t_chunk));
	uint32_t combined   = chunk->size + next_total;
	t_chunk *nnc        = get_next_chunk(heap, next_chunk);

	if (combined < (uint32_t)new_size)
        return (NULL);

    arena_fastbin_unlink(next_chunk);
	
	if (combined == (uint32_t)new_size) {

		if (nnc) {
			nnc->prev_size = (nnc->prev_size & NEXT_INUSE) + chunk->size;
			set_prev_in_use(nnc);
		}
		chunk->size += next_total;
	}
	else if (next_total >= MIN_TRIM) {

		t_chunk *new_free = heap_split_chunk(heap, chunk, new_size);
		nnc->prev_size = (nnc->prev_size & NEXT_INUSE) + new_free->size;
		chunk->size = new_size;
	}
	return (chunk);
}

t_chunk *heap_check_prev_chunk(t_heap *heap, t_chunk *chunk, size_t new_size) {

	t_chunk *prev = get_prev_chunk(heap, chunk);

	if (!prev)
		return (NULL);

	if (prev_in_use(chunk))
		return (NULL);

	uint32_t prev_total = (prev->size + sizeof(t_chunk));

	if (prev_total < (uint32_t)new_size)
		return (NULL);


	arena_fastbin_unlink(prev);
	t_chunk *new_chunk = prev;

	new_chunk->prev_size = (prev->prev_size & PREV_INUSE) + (prev->prev_size & FLAG_MASK);
	new_chunk->size = new_size;

	ft_memmove(chunk_to_data(prev), chunk_to_data(chunk), new_size);
	if (prev->size == (uint32_t)new_size) {

		arena_fastbin_set(heap, chunk);
	}
	else if (prev_total >= new_size && prev_total >= MIN_TRIM) {

		heap_split_chunk(heap, prev, new_size);

	}
	return (new_chunk);

}