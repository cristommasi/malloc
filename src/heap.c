#include "../include/malloc.h"

t_heap *heap_start = NULL;

t_heap    *heap_init(size_t size) {

    size_t  zone_size;
    t_heap  *new_heap;
    t_block *first_block;

    zone_size = heap_size(size);

    new_heap = (t_heap *)mmap(NULL, zone_size, PROT_FLAGS, MAP_FLAGS, NO_FD, NO_OFFSET);
    if (new_heap == MAP_FAILED) {
        return (NULL);
    }
    new_heap->group         = heap_group(size);
    new_heap->prev          = NULL;
    new_heap->next          = NULL;
    new_heap->total_size    = zone_size - sizeof(t_heap);
    new_heap->free_size     = zone_size - sizeof(t_heap);
    new_heap->block_count   = 0;


    first_block = heap_to_block(new_heap);

    first_block->prev = NULL;
    first_block->next = NULL;
    first_block->data_size = size + sizeof(t_block);
    first_block->freed = true;
    new_heap->block_count += 1;

    return (new_heap);
}

t_heap    *heap_new(size_t size) {

    size_t  zone_size = heap_size(size);

    t_heap *new_heap = (t_heap *)mmap(NULL, zone_size, PROT_FLAGS, MAP_FLAGS, NO_FD, NO_OFFSET);
    if (new_heap == MAP_FAILED) {
        return (NULL);
    }

    new_heap->group = heap_group(size);
    new_heap->prev  = heap_prev();
    new_heap->next = heap_next();
    new_heap->total_size = zone_size - sizeof(t_heap);
    new_heap->free_size = zone_size - sizeof(t_heap);
    new_heap->block_count = 0;
    return (new_heap);
}

t_heap  *heap_prev(void) {

    t_heap *cur = heap_start;

    if (cur == NULL) {
        return (NULL);
    }

    while (cur->next != NULL) {

        cur = cur->next;
    }
    return (cur);
}

t_heap  *heap_next(void) {

    t_heap *cur = heap_start;

    if (cur == NULL) {
        return (NULL);
    }
    while (cur->next != NULL) {

        cur = cur->next;
    }
    return (cur);
}

size_t  heap_size(size_t size) {

    if (size <= TINY_MAX) {
        return (TINY_BLOCK_SIZE);
    }
    else if (size <= SMALL_MAX) {
        return (SMALL_BLOCK_SIZE); 
    }
    else {
        return (size);
    }
}

t_head_group  heap_group(size_t size) {

    if (size <= TINY_MAX) {
        return (TINY);
    }
    else if (size <= SMALL_MAX) {
        return (SMALL); 
    }
    else {
        return (LARGE);
    }
}


