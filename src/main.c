#include "../include/malloc.h"

size_t align(size_t size)
{
    return ((size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
}

#include <stdint.h>
void printa(char *str, void *ptr) {printf(str, (unsigned long)(uintptr_t)ptr);}

void printALL(void) {


    t_heap *cur_heap = heap_start;
    char heap_name = 'A';

    printf("\n\n---------------------------------\n");
    while (cur_heap != NULL) {

        printf("CUR-HEAP   : %c (%p)\n", heap_name, cur_heap);
        printf("total_size : %ld\n", cur_heap->total_size);
        printf("free_size  : %ld\n", cur_heap->free_size);
        printf("block_count: %ld\n", cur_heap->block_count);


        printf("---------------------------------\n");
        t_block *cur_block = heap_to_block(cur_heap);
        void *heap_end = (void *)((char *)cur_heap + sizeof(t_heap) + cur_heap->total_size);
        char block_name = 'a';
        while ((void *)cur_block < heap_end) {

            if (cur_block->available)
                printf("FREE-BLOCK  : %c (%p)\n", block_name, cur_block);
            else {
                printf("USED-BLOCK  : %c (%p)\n", block_name, cur_block);
                char *data = block_to_data(cur_block);
                printf("data        : \"");
                size_t i = 0;
                while (i < cur_block->data_size) {
                	char c = data[i];
                	if (c == '\0')
                		break ;
                	printf("%c", c);
                	i++;
                }
                printf("\"\n");

            }
            printf("data_size   : %ld\n", cur_block->data_size);
            printf("---------------------------------\n");

            block_name += 1;
            cur_block = (void *)((char *)cur_block + sizeof(t_block) + cur_block->data_size);       
        }
        heap_name += 1;
        cur_heap = cur_heap->next;
    }
}

void    *ft_malloc(size_t size) {

    t_heap *heap = NULL;
    t_block *block = NULL;


    if (size == 0) return (NULL);
    printf("ft_mallloc called\n");
    size = align(size);

    heap = heap_find(size);
    if (heap == NULL)
        return (NULL);
    block = block_find(heap, size);
    if (block == NULL) {
        heap = heap_new(heap, size);
        block = block_find(heap, size);
    }
    printf("Returning block of data\n");
    return (block_to_data(block));
}



int main(void) {



    char *str = (char *)ft_malloc(6);
    ft_strlcpy(str, "Hello", 6);

    printALL();

    char *str2 = (char *)ft_malloc(7);
    ft_strlcpy(str2, " World", 7);


    printALL();

    char *str3 = (char *)ft_malloc(129);
    ft_strlcpy(str3, " GHIIIIIIIIIIIIII", 18);


    printALL();



    return 0;
}

