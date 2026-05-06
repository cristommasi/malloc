#include "./include/malloc.h"
#include <stdio.h>


int main(void) {

	char *heap = ft_malloc(16);
	// heap = heap + 2 * sizeof(char*);
	ft_memset(heap, 'B', 8);
	// heap = heap + 4 * sizeof(char*);
	show_alloc_mem();
    // [H][1]   ]
    return (0);
}

