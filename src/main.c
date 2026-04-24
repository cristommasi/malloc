#include "../include/malloc.h"

t_arena g_arena = {0};


void    *ft_malloc(size_t size) {

	t_heap  *heap  = NULL;
	t_chunk	*chunk = NULL;
	void	*ptr   = NULL;

	if (size == 0) {
		return (NULL);
	}
	size = ALIGN(size);

	if (arena_heap_uninitialized_or_large(size)) {

		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);

		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);

	}
	else if ((chunk = arena_fastbin_get(size)) != NULL) {

		if ((heap = arena_heap_find_by_chunk(chunk)) == NULL)
			return (NULL);
		heap->blocks += 1;

	}
	else if ((heap = heap_find_cis_mem(size)) == NULL) {

		if ((heap = heap_new_and_append(size)) == MAP_FAILED)
			return (NULL);

		if ((chunk = heap_split_cis_mem(heap, size)) == NULL)
			return (NULL);
	}
	ptr = chunk_to_data(chunk);
	return (ptr);
}

void    ft_free(void *ptr) {
	
	t_chunk *chunk      = NULL;
	t_heap  **heap_head = NULL;
	t_heap  *heap       = NULL;
	t_heap  *prev       = NULL;

	if (ptr == NULL || (chunk = data_to_chunk(ptr)) == NULL)
		return ;

	if ((heap_head = arena_heap_group(get_size(chunk))) == NULL)
		return ;

	heap = *heap_head;
	while (heap != NULL) {

		if (chunk_belongs_to_heap(heap, chunk)) {

			if (is_large(chunk)) {

				return arena_heap_munmap(prev, heap, heap_head);
			}
			else {

				arena_fastbin_set(heap, chunk);
				if (heap->blocks == 0) {

					arena_fastbin_drain(heap);
                    return arena_heap_munmap(prev, heap, heap_head);
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

    

    if (ptr == NULL) {

        return (ft_malloc(size));
	}
    else if ((chunk = data_to_chunk(ptr)) == NULL) {
		
        return (NULL);
	}
    else if ((heap = arena_heap_find_by_chunk(chunk)) == NULL) {
		
        return (NULL);
	}
    else if (size == 0) {
		
        return (ft_free(ptr), NULL);
	}
    else if ((cur_size = get_size(chunk)) == p_new_size) {
		
        return (ptr);
	}
    else if (heap_is_different_type(p_new_size, cur_size)) {
		
        return (arena_get_new_chunk(ptr, p_new_size, cur_size));
	}
	else if (p_new_size != cur_size && (chunk = heap_realloc_in_place(heap, chunk, p_new_size)) != NULL) {

		return (chunk_to_data(chunk));
	}
    return (arena_get_new_chunk(ptr, p_new_size, cur_size));

}

int main(void)
{

    
    char *s1 = NULL;
	
	s1 = (char*)ft_malloc(16);
	ft_memcpy(s1, "AAAAAAAAAAAAAA", 15);

	printf("str1 = %s\n", s1);

	s1 = (char*)ft_realloc(s1, 32);
	int i = 16;
	for (; i < 32; i++)
		s1[i] = 'B';
	s1[i] = '\0';
	printf("str1 = %s\n", s1);

    return (0);
}