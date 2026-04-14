#include "../include/malloc.h"


t_block *block_find(t_heap *heap, size_t size) {

	t_block *cur_block = heap_to_block(heap);

	while (cur_block != NULL) {

		if (cur_block->available == true && cur_block->data_size >= size) {

			return (cur_block);
		}
		else if (cur_block->available == true && cur_block->data_size < size) {

			block_merge(cur_block, heap);
			if (cur_block->data_size >= size) {
				return (cur_block);
			}
		}
		cur_block = cur_block->next;
	}
	return (NULL);
}

void	block_merge(t_block *block, t_heap *heap) {

	t_block *next_block = block->next;


	while (next_block != NULL && next_block->available == true) {

		next_block->prev->next = NULL;
		block->data_size += next_block->data_size + sizeof(t_block);
		next_block->data_size = 0;
		next_block->prev = NULL;
		next_block = next_block->next;


		heap->block_count -= 1;
		heap->free_size += block->data_size;
	}
	block->next = next_block;
}

void	block_split(t_block *block, size_t size) {
	
	t_block *new_block;

	if (block->data_size <= size + sizeof(t_block))
		return ;
	
	new_block = (t_block *)(block_to_data(block) + size);
	new_block->prev = block;
	new_block->next = NULL;
	new_block->data_size = block->data_size - sizeof(t_block) - size;
	new_block->available = true;

	block->next = new_block;
	block->data_size = size;
	block->available = false;
}

void    *block_to_data(t_block *block_addr) {

	return ((void *)(block_addr + 1));
}

void	*block_shift(t_block *block_addr) {

	return ((void *)(block_addr + sizeof(*block_addr)));
}


