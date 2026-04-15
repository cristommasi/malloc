#include "../include/malloc.h"

t_heap *heap_start = NULL;

t_heap    *heap_new(t_heap *prev, size_t size) {

    size_t  zone_size;
    t_heap  *new_heap = NULL;
    t_block *new_block = NULL;

    zone_size  = heap_size(size);
    new_heap = (t_heap *)mmap(NULL, zone_size + sizeof(t_heap), PROT_FLAGS, MAP_FLAGS, NO_FD, NO_OFFSET);
    if (new_heap == MAP_FAILED) {
        printf("mmap() FAILED\n\n");
        return (NULL);
    }
    new_heap->prev          = prev;
    new_heap->next          = NULL;
    new_heap->total_size    = zone_size;
    new_heap->free_size     = zone_size;
    new_heap->block_count   = 0;


    new_block = heap_to_block(new_heap);
    new_block->prev         = NULL;
	new_block->next         = NULL;
	new_block->data_size    = zone_size - sizeof(t_block);
    new_block->available    = true;

    new_heap->free_blocks  = new_block;
    if (prev)
        prev->next = new_heap;

    return (new_heap);
}


t_heap  *heap_find(size_t size) {

    t_heap          *cur_heap    = heap_start;
    t_heap          *prev_heap   = NULL;
    size_t          size_to_find = size + sizeof(t_block);
    bool            NO_HEAP = true;

    while (cur_heap != NULL) {

        NO_HEAP = false;
        printf("free = %zu - sizetofind = %zu\n", cur_heap->free_size, size_to_find);
        if (cur_heap->free_size >= size_to_find) {
            printf("Found heap with size req.\n");
            return (cur_heap);
        }
        prev_heap = cur_heap;
        cur_heap = cur_heap->next;
    }
    if (NO_HEAP) {
        printf("Initialized heap\n");
        heap_start = heap_new(NULL, size);
        return heap_start;
    }
    printf("Created new heap\n");
    return (heap_new(prev_heap, size));

}



size_t  heap_size(size_t size) {

    if (size <= TINY_MAX) {
        return (256);
    }
    else if (size <= SMALL_MAX) {
        return (SMALL_BLOCK_SIZE); 
    }
    else {
        return (size);
    }
}



t_block *heap_to_block(t_heap *heap_addr) {

	return ((t_block *)(heap_addr + 1));
}


