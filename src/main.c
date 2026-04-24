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
	else if (((chunk = heap_find_cis_mem_chunk(size)) == NULL)) {

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

	if (ptr == NULL)
		return ;

	if ((chunk = data_to_chunk(ptr)) == NULL)
		return ;

	if ((heap_head = arena_heap_group(get_size(chunk))) == NULL)
		return ;

	printf("chunk %p IN_USE flag = %d\n", chunk, has_flags(chunk, IN_USE));
	heap = *heap_head;

	printf("ft_free receives = chunk %p - data %p - free %p\n", (char*)chunk, (char*)ptr, (char*)heap->free_cis_start);
	while (heap != NULL) {

		if (chunk_belongs_to_heap(heap, chunk)) {

			if (is_large(chunk)) {

				arena_heap_munmap(prev, heap, heap_head);
				ptr = NULL;
				return ;
			}
			else {

				if (!has_flags(chunk, IN_USE))
					return;
				
				arena_fastbin_set(heap, chunk);
				ptr = NULL;
				if (heap->blocks == 0) {

					arena_fastbin_drain(heap);
                    arena_heap_munmap(prev, heap, heap_head);
					
					return ;
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
    if ((chunk = data_to_chunk(ptr)) == NULL) {
		
        return (NULL);
	}
    if ((heap = arena_heap_find_by_chunk(chunk)) == NULL) {
		
        return (NULL);
	}
	printf("ft_realloc receives = chunk %p - data %p - free %p\n", (char*)chunk, (char*)ptr, (char*)heap->free_cis_start);
	printf("new size = %zu\n", p_new_size);
	//printheap(heap, chunk);
    if (size == 0) {
		
        return (ft_free(ptr), NULL);
	}
    if ((cur_size = get_size(chunk)) == p_new_size) {
		
        return (ptr);
	}
    if (heap_is_different_type(p_new_size, cur_size)) {
		
        return (arena_get_new_chunk(ptr, p_new_size, cur_size));
	}
	else if (p_new_size != cur_size) {

		
		if ((chunk = heap_realloc_in_place(heap, chunk, p_new_size)) != NULL)
			return (chunk_to_data(chunk));
	}
    return (arena_get_new_chunk(ptr, p_new_size, cur_size));

}

int main(void)
{

    
    char *s1 = (char*)ft_malloc(32);
	char *s2 = (char*)ft_malloc(32);
	char *s3 = (char*)ft_malloc(32);
	char *s4 = (char*)ft_malloc(32);

	ft_free(s1);
	ft_free(s2);
	ft_free(s3);
	ft_free(s4);
	
	ft_free(s2);
	printf("\n");
	

    return (0);
}