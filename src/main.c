#include "../include/malloc.h"

void    *ft_malloc(size_t size) {

    t_heap  *heap  = NULL;
    t_block *block = NULL;


    printf("ft_mallloc called\n");

    if (size == 0)
        return (NULL);
    size = alignSize(size);

    // Initialize heap
    if (!heap_start) {
        if ((heap_start = heap_new(NULL, size)) == NULL)
            return (NULL);
    }

    // Find heap that has enough free size
    if ((heap = heap_find(size)) == NULL) {

        return (NULL);
    }
    // If enough free size but fragmented, create new heap
    if ((block = block_find(heap, size)) == NULL) {

         // SHOULDNT FAIL UNLESS mmap() fails
        if ((heap = heap_new(heap, size)) == NULL)
            return (NULL);
        if ((block = block_find(heap, size)) == NULL)
            return (NULL);
    }
    printf("Returning block of data\n");
    return (block_to_data(block));
}



int main(void) {



    char *str = (char *)ft_malloc(6);
    ft_strlcpy(str, "Hello", 6);

    //printALL();

    char *str2 = (char *)ft_malloc(7);
    ft_strlcpy(str2, " World", 7);


    //printALL();

    char *str3 = (char *)ft_malloc(129);
    ft_strlcpy(str3, " GHIIIIIIIIIIIIII", 18);


    //printALL();

    char *str4 = (char *)ft_malloc(1234567);
    ft_strlcpy(str4, "abcdefghijklmnopqrstuvwxyz", 27);


    printALL();



    return 0;
}


