#include "../include/malloc.h"

/*
typedef struct s_heap {

    size_t          total_size;
    size_t          free_size;
    struct s_heap   *prev;
    struct s_heap   *next;

}               t_heap;
*/

t_heap *heap_new_and_append(size_t size) {

    size_t  zone_size = heap_size(size);
    t_heap *new_heap  = heap_new(zone_size);
    
    if (new_heap == MAP_FAILED) {
        return (MAP_FAILED);
    }
    printf("%s - Allocated new %s heap\n", __func__, ((heap_type(size) == TINY_HEAP) ? "TINY" : ((heap_type(size) == SMALL_HEAP) ? "SMALL" : "LARGE")));
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

        printf("%s - mmap() FAILED\n", __func__);
        return (MAP_FAILED);
    }

    new_heap->total_size    = zone_size;
    new_heap->free_start    = heap_to_chunk(new_heap);
    new_heap->next          = NULL;
    return (new_heap);
}

void    heap_append(t_heap **HEAP_TYPE, t_heap *new_heap) {

    t_heap *cur = *HEAP_TYPE;
    t_heap *prev = NULL;

    printf("%s - Appended new heap\n", __func__);
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

    t_heap  *cur = arena_heap_group(size);


    while (cur != NULL) {

        if (heap_has_remaining_cis(cur, size)) {
            printf("%s - %s heap with size req.\n", __func__, ((heap_type(size) == TINY_HEAP) ? "TINY" : "SMALL"));
            return (cur);
        }
        cur = cur->next;
    }
    printf("%s - found no heap with size large enough\n", __func__);
    return (NULL);

}


t_chunk		*heap_split_cis_mem(t_heap *heap, size_t size) {

	t_chunk *new_use_chunk = heap->free_start;

	new_use_chunk->size = size;
	new_use_chunk->next = NULL;

    heap->free_start = (t_chunk *)((char *)heap->free_start + sizeof(t_chunk) + size);
	printf("%s - Split mem success.\n", __func__);
	return (new_use_chunk);
}



size_t  heap_size(size_t size) {

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

int		heap_has_remaining_cis(t_heap *heap, size_t size) {

	return (((char*)heap->free_start + size + sizeof(t_chunk)) <= (char *)heap_to_chunk(heap) + heap->total_size);
}

t_chunk *heap_to_chunk(t_heap *heap_addr) {

	return ((t_chunk *)((char *)heap_addr + sizeof(t_heap)));
}




