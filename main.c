#include "./include/malloc.h"

#include <stdio.h>

int main(void)
{
    char *lol[102];

    for (int i = 0; i < 102; i++) {
        lol[i] = ft_malloc(TINY_CHUNK_MAX);
        ft_memset(lol[i], 'A' + (i % 26), TINY_CHUNK_MAX);
    }
    ft_free(lol[0]);
    show_alloc_mem_ex(3);
    return (0);
}