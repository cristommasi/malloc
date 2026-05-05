#include "./include/malloc.h"
#include <stdio.h>

// ── MAIN ──────────────────────────────────────────────────────────────────────

int main(void) {

	char *heap = ft_malloc(16);
	// heap = heap + 2 * sizeof(char*);
	ft_memset(heap, 'B', 8);
	// heap = heap + 4 * sizeof(char*);
	show_alloc_mem_ex(0);
    // [H][1]   ]
    return (0);
}