#include "../include/malloc.h"

t_arena g_arena = {0};


void    *ft_malloc(size_t size) {

	t_heap  *heap;
	t_chunk *chunk;
	void *ptr = NULL;

	//printf("\n%s - Called----------------------\n", __func__);
	if (size == 0) {
		//printf("%s - Size is 0\n", __func__);
		return (NULL);
	}
	size_t sizeA = ALIGN(size);

	if (arena_heap_uninitialized_or_large(sizeA)) {

		if ((heap = heap_new_and_append(sizeA)) == MAP_FAILED)
			return (NULL);

		if ((chunk = heap_split_cis_mem(heap, sizeA)) == NULL)
			return (NULL);

	}
	else if ((chunk = arena_fastbin_get(size)) == NULL) {

		heap = heap_find_cis_mem(sizeA);
		if (!heap) {
			
			if ((heap = heap_new_and_append(sizeA)) == MAP_FAILED)
				return (NULL);
		}
		chunk = heap_split_cis_mem(heap, sizeA);

	}
	//printf("%s - Returning block of data\n\n", __func__);
	ptr = chunk_to_data(chunk);
	VALGRIND_MALLOCLIKE_BLOCK(ptr, size, 0, 0);
	VALGRIND_MAKE_MEM_UNDEFINED(ptr, size);
	return (ptr);
}

void    ft_free(void *ptr) {
	
	if (!ptr) {
		//printf("%s - ptr is null\n", __func__);
		return ;
	}
	
	t_chunk *chunk  = data_to_chunk(ptr);
	if (!chunk) {
		//printf("%s - chunk is null\n", __func__);
		return ;
	}
	t_heap  **heap_head = arena_heap_group(chunk->size);
	t_heap  *heap       = *heap_head;
	t_heap  *prev       = NULL;


	while (heap != NULL) {

		if (arena_owner_of_heap(heap, chunk)) {

			if (chunk_covers_entire_heap(heap, chunk)) {
				VALGRIND_FREELIKE_BLOCK(ptr, 0);
				if (arena_heap_munmap(prev, heap, heap_head) == -1)
					return ;//printf("%s - munmap failed\n", __func__);
				//printf("%s - munmap success\n", __func__);
				return ;
			}
			else {

				arena_fastbin_set(heap, chunk);
				if (heap->blocks == 0) {

					arena_fastbin_drain(heap);
					if (arena_heap_munmap(prev, heap, heap_head) == -1)
						return ;//printf("%s - munmap failed\n", __func__);
					//printf("%s - munmap success\n", __func__);
				}
				return ;
			}
		}
		prev = heap;
		heap = heap->next;
	}
}



int main(void)
{

	char *str = NULL;
	int i = 0;
	while (i < 10) {
		str = ft_malloc(10);
		ft_free(str);
		i++;
	}

	// ft_strlcpy(str, "helloo", 8);
	// printf("%s\n", str);
	// ft_free(str);

    return (0);
}