#include "../include/malloc.h"

size_t align(size_t size)
{
    return ((size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
}

#include <stdint.h>
void printa(char *str, void *ptr) {printf(str, (unsigned long)(uintptr_t)ptr);}

void printALL(void) {


    t_heap *cur_heap = heap_start;
    printf("---------------------------------\n");
    while (cur_heap != NULL) {

        t_block *cur_block;
        printf("CUR-HEAP   : ");printa("%ld\n", cur_heap);
        printf("group      : %d\n", cur_heap->group);
        printf("total_size : %ld\n", cur_heap->total_size);
        printf("free_size  : %ld\n", cur_heap->free_size);
        printf("block_count: %ld\n", cur_heap->block_count);
        printf("---------------------------------\n");
        while (cur_block != NULL) {

            
            printf("CUR-BLOCK: ");printa("%ld\n", cur_block);
            printf("available: %d\n", cur_block->available);
            printf("data_size: %ld\n", cur_block->data_size);
            printf("---------------------------------\n");

            
            cur_block = cur_block->next;
        }

        cur_heap = cur_heap->next;
    }
}

void    *ft_malloc(size_t size) {

    t_heap *heap;
    t_block *block;


    if (size == 0)
        return (NULL);

    if (heap_start == NULL) {
        heap_start = heap_new(size);
    }
    size = align(size);
    heap = heap_find(size);
    if (heap == NULL) {
        heap = heap_new(size);
        heap_start->next = heap;
    }
    block = block_find(heap, size);
    if (block == NULL) {
        heap = heap_new(size);
        block = block_find(heap, size);
    }
    block_split(block, size);
    return (block_to_data(block));
}



int main(void) {

    printALL();
    char *str = (char *)ft_malloc(14);
    int i = 0;
    for (; i < 15; i++) {
        str[i] = i + 'A';
    }
    str[i] = '\0';
    printf("\n\n");
    char *str2 = (char *)ft_malloc(1000);
    for (i = 0; i < 15; i++) {
        str2[i] = i + 'a';
    }
    str2[i] = '\0';
    printALL();
    return 0;
}

