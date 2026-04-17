
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

bool        arena_heap_uninitialized_or_large(size_t size) {

    t_heap_type TYPE = heap_type(size);


    if ((TYPE == LARGE_HEAP) || (!g_arena.tiny && TYPE == TINY_HEAP) || (!g_arena.small && TYPE == SMALL_HEAP)) {
        printf("%s - No heaps found initially or request heap is large\n", __func__);
        return (true);
    }

    printf("%s - Heaps found initially\n", __func__);
    return (false);
}



t_chunk     *arena_fastbin_get(size_t size) {

    size_t  index        = BIN_IDX(size + sizeof(t_chunk));
    t_chunk *chunk       = g_arena.fastbin[index];

    if (!chunk)
        return (NULL);
    g_arena.fastbin[index] = chunk->next;
    chunk->next = NULL;
	return (chunk);
}

void    arena_fastbin_set(t_chunk *freed_chunk) {

	size_t	index = BIN_IDX(freed_chunk->size + sizeof(t_chunk));

	freed_chunk->next = g_arena.fastbin[index];
	g_arena.fastbin[index] = freed_chunk;
}



t_heap      *arena_heap_group(size_t size) {

    size_t zone_size = heap_size(size);

    if (zone_size == TINY_HEAP_SIZE) {

        return (g_arena.tiny);
    }
    else if (zone_size == SMALL_HEAP_SIZE) {

        return (g_arena.small);
    }
    else {
        return (g_arena.large);
    }
    
}
