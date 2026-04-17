#include "../include/malloc.h"

t_arena g_arena = {0};


void    *ft_malloc(size_t size) {

    t_heap  *heap;
    t_chunk *chunk;

    printf("\n%s - Called----------------------\n", __func__);
    if (size == 0) {
        printf("%s - Size is 0\n", __func__);
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
    else {

        chunk = arena_fastbin_get(size);
        if (!chunk) {
    
            heap = heap_find_cis_mem(size);
            if (!heap) {
                
                if ((heap = heap_new_and_append(size)) == MAP_FAILED)
                    return (NULL);
            }
            chunk = heap_split_cis_mem(heap, size);
        }
    }
    printf("%s - Returning block of data\n\n", __func__);
    return (chunk_to_data(chunk));
}

void    ft_free(void *ptr) {
    
    t_chunk *chunk = (t_chunk*)ptr;

    if (!chunk)
        return (NULL);
    t_heap_type  TYPE = heap_type(heap_size(chunk->size));

    if (TYPE == )

}


int main(void) {

    for (size_t i = 0; i < 64; i++) {

        printf("[%zu] = %zu\n", i , ((i + 1) * 16));

    }

    // char *str = (char *)ft_malloc(12);
    // ft_strlcpy(str, "Hello", 6);

    // //printALL();

    // char *str2 = (char *)ft_malloc(7);
    // ft_strlcpy(str2, " World", 7);


    // //printALL();

    // char *str3 = (char *)ft_malloc(129);
    // ft_strlcpy(str3, " GHIIIIIIIIIIIIII", 18);


    // //printALL();

    // char *str4 = (char *)ft_malloc(1234567);
    // ft_strlcpy(str4, "abcdefghijklmnopqrstuvwxyz", 27);


    // printALL();



    return 0;
}


