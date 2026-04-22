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
	size = ALIGN(size);

	if (arena_heap_uninitialized_or_large(size)) {

		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);

		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);

	}
	else if ((chunk = arena_fastbin_get(size)) == NULL) {


		heap = heap_find_cis_mem(size);
		if (!heap) {
			
			if ((heap = heap_new_and_append(size)) == MAP_FAILED)
				return (NULL);
		}
		chunk = heap_split_cis_mem(heap, size);

	}
	//printf("%s - Returning block of data\n\n", __func__);
	ptr = chunk_to_data(chunk);
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
	t_heap  **heap_head = arena_heap_group((size_t)chunk->size);
	t_heap  *heap       = *heap_head;
	t_heap  *prev       = NULL;


	while (heap != NULL) {

		if (chunk_belongs_to_heap(heap, chunk)) {

			if (heap_type(chunk->size) == LARGE_HEAP) {

                int ret = arena_heap_munmap(prev, heap, heap_head);
				if (ret == -1)
					printf("%s - munmap failed\n", __func__);
				return ;
			}
			else {

				arena_fastbin_set(heap, chunk);
				if (heap->blocks == 0) {

					arena_fastbin_drain(heap);
                    int ret = arena_heap_munmap(prev, heap, heap_head);
					if (ret == -1)
						printf("%s - munmap failed\n", __func__);
				}
				return ;
			}
		}
		prev = heap;
		heap = heap->next;
	}
}

void    *ft_realloc(void *ptr, size_t size) {
    
    t_chunk     *chunk       = NULL;
    t_heap      *heap        = NULL;
    size_t      p_new_size   = ALIGN(size);
    size_t      cur_size     = 0;

    

    if (ptr == NULL)
        return (ft_malloc(size));

    if ((chunk = data_to_chunk(ptr)) == NULL)
        return (NULL);

    // if not owners of heap
    if ((heap = arena_heap_find_by_chunk(chunk)) == NULL)
        return (NULL);

    if (size == 0)
        return (ft_free(ptr), NULL);

    cur_size = (size_t)chunk->size;
    if (p_new_size == cur_size) {
        return (ptr);
    }
    else if (heap_is_different_type(p_new_size, cur_size)) {

        return (arena_get_new_chunk(ptr, p_new_size, cur_size));
    }
    else if (p_new_size < cur_size && p_new_size >= MIN_TRIM) {

        heap_split_chunk(heap, chunk, p_new_size);
        return (chunk_to_data(chunk));
    }
    else if (p_new_size > cur_size) {

        chunk = heap_check_next_chunk(heap, chunk, p_new_size);
        if (!chunk) {

            chunk = heap_check_prev_chunk(heap, chunk, p_new_size);
            if (!chunk)
                return (arena_get_new_chunk(ptr, p_new_size, cur_size));

            return (chunk_to_data(chunk));
        }
        return (chunk_to_data(chunk));
    }
    else { // get new chunk

        return (arena_get_new_chunk(ptr, p_new_size, cur_size));
    }
    return (NULL);
}

int main(void)
{
	t_chunk lol;

	lol.next = NULL;
	lol.TYPE = SMALL_HEAP;
	lol.vars.info.size = 5;

	printf("su %zu\n", sizeof(t_chunk));
    // char *s1 = ft_malloc(64);

    // char *s2 = ft_malloc(64);

    // int i = 0;
    // for ( ; i < 64; i++) {

    //     s1[i] = 'A' + (i % 26);
    // }
    // s1[i] = 0;
    // printf("str = %s\n", s1);

    // ft_free(s2);
    // s1 = ft_realloc(s1, 128);
    // i = 64;
    // for ( ; i < 128; i++) {

    //     s1[i] = 'a' + (i % 26);
    // }
    // s1[i] = 0;
    // printf("str = %s\n", s1);
    return (0);
}