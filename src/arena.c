
#include "../include/malloc.h"

/*
typedef struct s_arena {
    t_heap  *TINY;
    t_heap  *SMALL;
    t_heap  *LARGE;
    t_chunk *fastbin[8]; // array of s-linked-list of sizes 32, 48, 64, 80, 96, 112, 128 (LIFO)
    t_chunk *smallbin;  // circular d-linked list ordered by size (FIFO)
}               t_arena;
*/

bool        arena_heap_initialized(size_t size) {

    t_heap_type TYPE = heap_type(size);


    if ((TYPE == LARGE_HEAP) || (!g_arena.TINY && TYPE == TINY_HEAP) || (!g_arena.SMALL && TYPE == SMALL_HEAP)) {
        printf("%s - No heaps found initially or request heap is large\n", __func__);
        return (false);
    }

    printf("%s - Heaps found initially\n", __func__);
    return (true);
}

t_chunk     *arena_bin_find(size_t size) {

	t_heap_type HEAP_TYPE    = heap_type(heap_size(size));
	size_t		size_to_find = size + sizeof(t_chunk);
	t_chunk	    *chunk		 = NULL;

	if (HEAP_TYPE == TINY_HEAP) {

		return (arena_fastbin_find(size_to_find));
	}
	else if (HEAP_TYPE == SMALL_HEAP) {

		return (arena_smallbin_find(size_to_find));

	}
	return (NULL);
}

t_chunk     *arena_fastbin_find(size_t size) {

    size_t  size_to_find = size + sizeof(t_chunk);
    int     index        = ((size_to_find - FASTBIN_MIN_CHUNK) / ALIGNMENT);
    t_chunk *chunk       = g_arena.fastbin[index];

    if (!chunk)
        return (NULL);
    g_arena.fastbin[index] = chunk->next;
    chunk->next = NULL;
    chunk->prev = NULL;
    return (chunk);
}


t_chunk     *arena_smallbin_find(size_t size) {

    size_t  size_to_find = size + sizeof(t_chunk);
    t_chunk *cur = g_arena.smallbin;
    
    while (cur != NULL) {

        if (cur->size == size_to_find)
            break;
        cur = cur->next;
        if (cur == g_arena.smallbin)
            break;
    }
    cur->next->prev = cur->prev;
    cur->prev->next = cur->next;
    cur->next = NULL;
    cur->prev = NULL;
    return (cur);
}

t_heap      *arena_heap_group(size_t size) {

    size_t zone_size = heap_size(size);

    if (zone_size == TINY_HEAP_SIZE) {

        return (&g_arena.TINY);
    }
    else if (zone_size == TINY_HEAP_SIZE) {

        return (&g_arena.SMALL);
    }
    else {
        return (&g_arena.LARGE);
    }
    
}
