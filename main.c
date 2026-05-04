#include "./include/malloc.h"
#include <stdio.h>

// ── MAIN ──────────────────────────────────────────────────────────────────────

int main(void) {

	int *heap2 = ft_malloc(16);
	int *heap = ft_malloc(16);
	// heap = heap + 2 * sizeof(char*);
	// ft_memset(heap, 'B', 8);
	// heap = heap + 4 * sizeof(char*);
	// ft_memset(heap, 'C', 8);
	ft_free(heap);
	show_alloc_mem_ex();
    // [H][1]   ]
    return (0);
}