#include "../include/malloc.h"


t_block *block_find(t_heap *heap, size_t size) {

	t_block *cur_block = heap->free_blocks;
	t_block *prev_block = NULL;

	while (cur_block != NULL) {

		// if requirements met, dettach and relink prev & next
		if (cur_block->data_size >= size) {

			size_t remaining = cur_block->data_size - size;


			// if there isnt a matching size, split block and create new one
			if (remaining > sizeof(t_block)) {

				printf("Slitting block of size: %zu to (%zu)-(%zu)\n", cur_block->data_size, size, remaining - sizeof(t_block));
				t_block *new_block = (t_block *)((char*)cur_block + size + sizeof(t_block));

				new_block->data_size = remaining - sizeof(t_block);
				new_block->available = true;
				new_block->next = cur_block->next;
				new_block->prev = prev_block;
				if (prev_block)
					prev_block->next = new_block;
				else
					heap->free_blocks = new_block;
				if (cur_block->next != NULL)
					cur_block->next->prev = prev_block;
				
			}
			else {

				if (prev_block)
					prev_block->next = cur_block->next;
				else
					heap->free_blocks = cur_block->next;
				if (cur_block->next != NULL)
					cur_block->next->prev = prev_block;
			}

			cur_block->data_size = size;
			cur_block->available = false;
			cur_block->next = NULL;
			cur_block->prev = NULL;
			heap->free_size -= (size + sizeof(t_block));
			heap->block_count += 1;

			return (cur_block);
		}
		prev_block = cur_block;
		cur_block = cur_block->next;
	}
	printf("Couldn't find single block that meets size\n");
	return (NULL);
}

void    *block_to_data(t_block *block_addr) {

	return ((void *)(block_addr + 1));
}




