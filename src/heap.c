#include "../include/malloc.h"

t_heap *heap_start = NULL;

t_heap    *heap_new(size_t size) {

    size_t  zone_size;
    t_heap  *new_heap;
    t_block *first_block;

    zone_size = heap_size(size);

    new_heap = (t_heap *)mmap(NULL, zone_size + sizeof(t_heap), PROT_FLAGS, MAP_FLAGS, NO_FD, NO_OFFSET);
    if (new_heap == MAP_FAILED) {
        return (NULL);
    }
    new_heap->group         = (t_head_group)heap_group(size);
    new_heap->prev          = heap_prev();
    new_heap->next          = NULL;
    new_heap->total_size    = zone_size;
    new_heap->free_size     = zone_size;
    new_heap->block_count   = 1;

    first_block = heap_to_block(new_heap);
    first_block->prev = NULL;
	first_block->next = NULL;
	first_block->data_size = zone_size - sizeof(t_block);
	first_block->available = true;


    return (new_heap);
}


t_heap  *heap_find(size_t size) {

    t_heap          *cur_heap     = heap_start;
    t_head_group    group_to_find = heap_group(size);

    while (cur_heap != NULL) {

        if (cur_heap->group == group_to_find) {
            return (cur_heap);
        }
        cur_heap = cur_heap->next;
    }
    return (NULL);

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
    while (cur != NULL) {

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

t_block *heap_to_block(t_heap *heap_addr) {

	return ((t_block *)(heap_addr + 1));
}


