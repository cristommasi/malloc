#include "../include/malloc.h"

t_block *heap_to_block(t_heap *heap_addr)
{
    return ((t_block *)(heap_addr + 1));
}

void    *block_to_data(t_block *block_addr) {

    return ((void *)(block_addr + 1));
}