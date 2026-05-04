#include "./include/malloc.h"
#include <stdio.h>

// ── MAIN ──────────────────────────────────────────────────────────────────────

int main(void) {


	int *heap = ft_malloc(127 * sizeof(int));
	ft_memset(heap, 123, 1);
	// heap = heap + 2 * sizeof(char*);
	// ft_memset(heap, 'B', 8);
	// heap = heap + 4 * sizeof(char*);
	// ft_memset(heap, 'C', 8);
	show_alloc_mem_ex();
    
    return (0);
}