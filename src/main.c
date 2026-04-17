#include "../include/malloc.h"

t_arena g_arena = {0};


void    *ft_malloc(size_t size) {

	t_heap  *heap;
	t_chunk *chunk;

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
 
		return (chunk_to_data(chunk));
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
	return (chunk_to_data(chunk));
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

#define N_TINY_MAX 128      // chunks to fill one tiny heap
#define TINY_FULL  112      // 112 + 16 (t_chunk) = 128 bytes per slot
#define TINY_PAD   8        // should align up to 16
#define TINY_MID   48       // middle size for fastbin variety

int main(void)
{
    char    *tiny_full[N_TINY_MAX];
    char    *tiny_mid[64];
    char    *tiny_pad[16];
    char    buf[128];
    int     i;

    // ═══════════════════════════════════════════════════════
    printf("═══ [1] fill one entire tiny heap (112 bytes x128) ═══\n");
    // ═══════════════════════════════════════════════════════
    i = 0;
    while (i < N_TINY_MAX) {
        tiny_full[i] = ft_malloc(TINY_FULL);
        ft_snprintf(buf, sizeof(buf), "full-%03d", i);
        ft_strlcpy(tiny_full[i], buf, TINY_FULL);
        i++;
    }
    printALL();

    // ═══════════════════════════════════════════════════════
    printf("\n═══ [2] alloc 8 bytes → expect chunk size 16 (padding) ═══\n");
    // ═══════════════════════════════════════════════════════
    i = 0;
    while (i < 16) {
        tiny_pad[i] = ft_malloc(TINY_PAD);
        ft_strlcpy(tiny_pad[i], "pad", 4);
        i++;
    }
    printALL();

    // ═══════════════════════════════════════════════════════
    printf("\n═══ [3] force a second tiny heap (alloc 64 more x48) ═══\n");
    // ═══════════════════════════════════════════════════════
    i = 0;
    while (i < 64) {
        tiny_mid[i] = ft_malloc(TINY_MID);
        ft_snprintf(buf, sizeof(buf), "mid-%03d", i);
        ft_strlcpy(tiny_mid[i], buf, TINY_MID);
        i++;
    }
    printALL();

    // ═══════════════════════════════════════════════════════
    printf("\n═══ [4] free every other full chunk → fill fastbin[112] ═══\n");
    // ═══════════════════════════════════════════════════════
    i = 0;
    while (i < N_TINY_MAX) {
        if (i % 2 == 0)
            ft_free(tiny_full[i]);
        i++;
    }
    printALL();

    // ═══════════════════════════════════════════════════════
    printf("\n═══ [5] free every other mid chunk → fill fastbin[48] ═══\n");
    // ═══════════════════════════════════════════════════════
    i = 0;
    while (i < 64) {
        if (i % 2 == 0)
            ft_free(tiny_mid[i]);
        i++;
    }
    printALL();

    // ═══════════════════════════════════════════════════════
    printf("\n═══ [6] reallocate from fastbin (should reuse, no new heaps) ═══\n");
    // ═══════════════════════════════════════════════════════
    char *reused[32];
    i = 0;
    while (i < 32) {
        reused[i] = ft_malloc(TINY_FULL);
        ft_strlcpy(reused[i], "recycled", 9);
        i++;
    }
    printALL();

    // ═══════════════════════════════════════════════════════
    printf("\n═══ [7] free remaining full + reused → heap[0] should munmap ═══\n");
    // ═══════════════════════════════════════════════════════
    i = 1;
    while (i < N_TINY_MAX) {    // odd ones still alive
        if (i % 2 == 1)
            ft_free(tiny_full[i]);
        i++;
    }
    i = 0;
    while (i < 32) {
        ft_free(reused[i]);
        i++;
    }
    printALL();

    // ═══════════════════════════════════════════════════════
    printf("\n═══ [8] free all pad + mid → all heaps should munmap ═══\n");
    // ═══════════════════════════════════════════════════════
    i = 0;
    while (i < 16) {
        ft_free(tiny_pad[i]);
        i++;
    }
    i = 1;
    while (i < 64) {    // odd ones still alive
        if (i % 2 == 1)
            ft_free(tiny_mid[i]);
        i++;
    }
    printALL();

    return (0);
}