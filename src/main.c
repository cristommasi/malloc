#include "../include/malloc.h"

size_t align(size_t size)
{
    return ((size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
}

void    *ft_malloc(size_t size) {

    t_heap *heap;
    t_block *block;


    if (size == 0)
        return (NULL);

    size = align(size);
    if ((heap = heap_find(size)) == NULL) {
        heap = heap_new(size);
    }
    
    if ((block = block_find(heap, size)) == NULL) {
        heap = heap_new(size);
        block = block_find(heap, size);
    }
    block_split(block, size);
    return ((void *)block);
}


int main(void) {

    char *str = (char *)ft_malloc(6);
    int i = 0;
    for (; i < 17; i++) {
        str[i] = i + 'A';
    }
    str[i] = '\0';
    printf("%ld\n", ft_strlen(str));
    return 0;
}
