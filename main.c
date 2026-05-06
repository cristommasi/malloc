#include "./include/malloc.h"
#include <stdio.h>


int main(void) {

	char **heap = ft2_malloc(128 * (sizeof(char*) + 1));
	int i = 0;
	for (; i < 128; i++) {

		heap[i] = ft2_malloc(120);
	}
	heap[i] = NULL;

	char **heap2 = ft2_malloc(128 * (sizeof(char*) + 1));
	i = 0;
	for (; i < 128; i++) {

		heap2[i] = ft2_malloc(1016);
	}
	heap2[i] = NULL;
	// heap = heap + 4 * sizeof(char*);
	show_alloc_mem_ex();

    // [H][1]   ]
    return (0);
}

