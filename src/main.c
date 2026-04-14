#include "../include/malloc.h"
#include <stdbool.h>

#define HEAP_SHIFT(start)   ((void*)start + sizeof(t_heap))
#define BLOCK_SHIFT(start)  ((void*)start + sizeof(t_block))


#define TINY_MAX        128              // max bytes for a tiny request
#define SMALL_MAX       1024             // max bytes for a small request

#define TINY_BLOCK_SIZE       (4 * PAGE_SIZE)  // 16384 - fits 128 tiny allocs
#define SMALL_BLOCK_SIZE      (32 * PAGE_SIZE) // 131072 - fits 128 small allocs

// Heap group size
typedef enum s_head_group { 
    TINY, 
    SMALL, 
    LARGE
} t_head_group;

// // Metadata for a whole mmap'd region
typedef struct s_heap {

    struct s_heap   *prev;
    struct s_heap   *next;
    t_head_group    group;
    size_t          total_size;
    size_t          free_size;
    size_t          block_count;

}               t_heap;


// Metadata for a single allocated block
typedef struct s_block {

    struct s_block  *prev;
    struct s_block  *next;
    size_t          data_size;
    bool            freed;

}               t_block;


t_heap *heap_start = NULL;

size_t  get_heap_size(size_t size) {

    if (size <= TINY_MAX) {
        return (TINY_BLOCK_SIZE);
    }
    else if (size <= SMALL_MAX) {
        return (SMALL_BLOCK_SIZE); 
    }
    else {
        return (size);
    }
}

t_head_group  get_heap_group(size_t size) {

    if (size <= TINY_MAX) {
        return (TINY);
    }
    else if (size <= SMALL_MAX) {
        return (SMALL); 
    }
    else {
        return (LARGE);
    }
}

t_heap  *get_prev_heap(void) {

    t_heap *cur = heap_start;

    if (cur == NULL) {
        return (NULL);
    }

    while (cur->next != NULL) {

        cur = cur->next;
    }
    return (cur);
}

t_heap  *get_next_heap(void) {

    t_heap *cur = heap_start;

    if (cur == NULL) {
        return (NULL);
    }
    while (cur->next != NULL) {

        cur = cur->next;
    }
    return (cur);
}

void    *get_new_heap_zone(size_t size) {

    size_t  zone_size = get_heap_size(size);

    t_heap *new_heap = (t_heap *)mmap(NULL, zone_size, PROT_FLAGS, MAP_FLAGS, NO_FD, NO_OFFSET);
    if (new_heap == MAP_FAILED) {
        return (NULL);
    }

    new_heap->group = get_heap_group(size);
    new_heap->prev  = get_prev_heap();
    new_heap->next;
    new_heap->total_size = zone_size - sizeof(t_heap);
    new_heap->free_size = zone_size - sizeof(t_heap);
    new_heap->block_count = 0;
    return (new_heap);
}

t_block *get_first_block(t_heap *heap)
{
    return (t_block *)(heap + 1);
}


t_heap    *init_heap(size_t size) {

    size_t  zone_size;
    t_heap  *new_heap;
    t_block *first_block;

    zone_size = get_heap_size(size);

    new_heap = (t_heap *)mmap(NULL, zone_size, PROT_FLAGS, MAP_FLAGS, NO_FD, NO_OFFSET);
    if (new_heap == MAP_FAILED) {
        return (NULL);
    }
    new_heap->group         = get_heap_group(size);
    new_heap->prev          = NULL;
    new_heap->next          = NULL;
    new_heap->total_size    = zone_size - sizeof(t_heap);
    new_heap->free_size     = zone_size - sizeof(t_heap);
    new_heap->block_count   = 1; //counting first block


    first_block = get_first_block(new_heap);

    first_block->prev = NULL;
    first_block->next = NULL;
    first_block->data_size = new_heap->total_size - sizeof(t_block);
    first_block->freed = true;

    return (new_heap);
}

void    *ft_malloc(size_t size) {

    if (heap_start == NULL) {

        heap_start = init_heap(size);
        if (heap_start == NULL) {
            return (NULL);
        }
    }



   
    
}

int main(void) {

    char *str = ft_malloc(5);


    return 0;
}
