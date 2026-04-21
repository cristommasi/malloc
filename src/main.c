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
    
    void        *new_ptr     = NULL;
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
    else if (p_new_size < cur_size && p_new_size >= MIN_TRIM) {

        if (heap_is_different_type(p_new_size, cur_size))
            return (arena_get_new_chunk(ptr, p_new_size, cur_size));
        chunk = heap_split_free_chunk(heap, chunk, p_new_size);
        return (chunk_to_data(chunk));
    }
    else if (p_new_size > cur_size) {

        if (heap_is_different_type(p_new_size, cur_size))
            return (arena_get_new_chunk(ptr, p_new_size, cur_size));

        chunk = heap_check_adjacent_chunks(chunk, p_new_size);
        if (!chunk)
            return (arena_get_new_chunk(ptr, p_new_size, cur_size));
        return (chunk_to_data(chunk));
    }
    else { // get new chunk

        return (arena_get_new_chunk(ptr, p_new_size, cur_size));
    }
    return (NULL);
}

int main(void)
{
    char *p;

    printf("═══ [1] basic alloc/free ═══\n");
    {
        char *a = ft_malloc(1);
        char *b = ft_malloc(7);
        char *c = ft_malloc(8);
        char *d = ft_malloc(9);
        *a = 'x';
        *b = 'y';
        ft_strlcpy(c, "hello123", 8);
        ft_strlcpy(d, "world", 6);
        ft_free(a);
        ft_free(b);
        ft_free(c);
        ft_free(d);
    }

    printf("═══ [2] free NULL ═══\n");
    {
        ft_free(NULL);
    }

    printf("═══ [3] alloc zero ═══\n");
    {
        p = ft_malloc(0);
        if (p != NULL)
            printf("FAIL - expected NULL\n");
    }

    printf("═══ [4] fill tiny heap exactly, free all ═══\n");
    {
        char *ptrs[128];
        char  buf[16];
        for (int i = 0; i < 128; i++) {
            ptrs[i] = ft_malloc(112);
            ft_strlcpy(buf, "block", 6);
            ft_strlcpy(ptrs[i], buf, 112);
        }
        for (int i = 0; i < 128; i++)
            ft_free(ptrs[i]);
    }

    printf("═══ [5] fastbin reuse ═══\n");
    {
        char *a = ft_malloc(48);
        ft_strlcpy(a, "first", 6);
        void *addr = a;
        ft_free(a);
        char *b = ft_malloc(48);
        ft_strlcpy(b, "reused", 7);
        if (b != addr)
            printf("NOTE - fastbin did not reuse\n");
        ft_free(b);
    }

    printf("═══ [6] interleaved alloc/free ═══\n");
    {
        char *ptrs[32];
        for (int i = 0; i < 32; i++) {
            ptrs[i] = ft_malloc(64);
            ft_strlcpy(ptrs[i], "data", 5);
        }
        for (int i = 1; i < 32; i += 2)
            ft_free(ptrs[i]);
        for (int i = 1; i < 32; i += 2) {
            ptrs[i] = ft_malloc(64);
            ft_strlcpy(ptrs[i], "refill", 7);
        }
        for (int i = 0; i < 32; i++)
            ft_free(ptrs[i]);
    }

    printf("═══ [7] mixed tiny + small, reverse free ═══\n");
    {
        char *t1 = ft_malloc(16);   ft_strlcpy(t1, "tiny1",  6);
        char *t2 = ft_malloc(64);   ft_strlcpy(t2, "tiny2",  6);
        char *s1 = ft_malloc(256);  ft_strlcpy(s1, "small1", 7);
        char *t3 = ft_malloc(112);  ft_strlcpy(t3, "tiny3",  6);
        char *s2 = ft_malloc(1024); ft_strlcpy(s2, "small2", 7);
        char *t4 = ft_malloc(8);    ft_strlcpy(t4, "t4",     3);
        ft_free(t4);
        ft_free(s2);
        ft_free(t3);
        ft_free(s1);
        ft_free(t2);
        ft_free(t1);
    }

    printf("═══ [8] large alloc ═══\n");
    {
        char *big = ft_malloc(1025);
        ft_strlcpy(big, "large block data", 17);
        ft_free(big);
    }

    printf("═══ [9] many small allocs, non-sequential free ═══\n");
    {
        char *ptrs[64];
        char  buf[16];
        for (int i = 0; i < 64; i++) {
            ptrs[i] = ft_malloc(512);
            ft_strlcpy(buf, "small", 6);
            ft_strlcpy(ptrs[i], buf, 512);
        }
        int order[] = {
             5, 12,  0, 31, 63,  7, 44, 19,
             2, 58, 33, 11, 47, 25,  8, 39,
            17, 53,  4, 28, 61, 14, 36, 22,
            49,  1, 41, 10, 55, 30, 18, 62,
             6, 45, 23, 38, 13, 57, 29,  3,
            50, 21, 34,  9, 60, 16, 42, 26,
            52, 37, 15, 48, 24, 40, 56, 32,
            20, 43, 59, 27, 35, 46, 51, 54
        };
        for (int i = 0; i < 64; i++)
            ft_free(ptrs[order[i]]);
    }

    printf("═══ [10] boundary writes ═══\n");
    {
        char *p8   = ft_malloc(8);
        char *p16  = ft_malloc(16);
        char *p112 = ft_malloc(112);
        p8[0]     = 'a';  p8[7]     = 'z';
        p16[0]    = 'a';  p16[15]   = 'z';
        p112[0]   = 'a';  p112[111] = 'z';
        ft_free(p8);
        ft_free(p16);
        ft_free(p112);
    }

    printf("═══ [11] stress — many sizes, random free pattern ═══\n");
    {
        char *ptrs[16];
        int   sizes[16] = {8, 16, 24, 32, 48, 64, 72, 80,
                           96, 112, 128, 200, 256, 512, 768, 1024};
        for (int i = 0; i < 16; i++) {
            ptrs[i] = ft_malloc(sizes[i]);
            ft_strlcpy(ptrs[i], "stress", 7);
        }
        // free in non-sequential order
        int order[] = {3, 7, 0, 12, 5, 15, 9, 1,
                       11, 4, 14, 6, 2, 10, 8, 13};
        for (int i = 0; i < 16; i++)
            ft_free(ptrs[order[i]]);
    }

    printf("═══ [12] arena state check ═══\n");
    {
        if (g_arena.tiny || g_arena.small || g_arena.large)
            printf("FAIL - heaps still mapped\n");
        else
            printf("OK   - all heaps munmapped\n");
        int dirty = 0;
        for (int i = 0; i < FASTBIN_COUNT; i++)
            if (g_arena.fastbin[i]) dirty++;
        if (dirty)
            printf("FAIL - %d fastbin slots not empty\n", dirty);
        else
            printf("OK   - fastbin clean\n");
    }

    return (0);
}