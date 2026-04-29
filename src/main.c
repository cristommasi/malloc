#include "../include/malloc.h"

#include <limits.h>

void	free_all(char **ptr, int idx) {

	if (!ptr)
		return ;

	for (int i = 0; i < idx; i++) {
		ft_free(ptr[i]);
	}
	ft_free(ptr);
}

char **mallocmany(size_t max_allocs, size_t m_size) {

	char **new = ft_malloc((max_allocs + 1) * (sizeof(char*)));
	if (!new) {
		printf("[mallocmany] **new FAILED\n");
		return (NULL);
	}

	size_t i = 0;

	while (i < max_allocs) {

		new[i] = ft_malloc(m_size);
		if (!new[i]) {
			printf("[mallocmany] new[i] FAILED at i=%zu\n", i);
			free_all(new, i);
			return (NULL);
		}
		i++;
	}
	new[i] = NULL;
	return (new);
}


int main(void)
{


	char **tiny = mallocmany(128, 112);
	if (!tiny) {
		return (1);
	}

	char **small = mallocmany(128, 1008);
	if (!small)
		return (1);


	char **large = mallocmany(5, 1009);
	if (!large)
		return (1);


	show_alloc_mem();

	printf("\n\n");

	printf("AFTER FREE \n");
	free_all(tiny, TINY_SMALL_BLOCK_MAX);
	free_all(small, TINY_SMALL_BLOCK_MAX);
	free_all(large, 5);
	// free_all(small, TINY_SMALL_BLOCK_MAX);
	// free_all(large, 5);

	
	show_alloc_mem();

    return (0);
}