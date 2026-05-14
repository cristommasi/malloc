#ifndef TEST_VALGRIND_MALLOC_PROJECT
# define TEST_VALGRIND_MALLOC_PROJECT

#include "../include/malloc.h"
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#include <assert.h>

void    *ft_malloc_vg(size_t size)
{
    void *ptr = ft_malloc(size);
    if (ptr) {
        if (size == 0)
            size = 16;
        VALGRIND_MALLOCLIKE_BLOCK(ptr, ALIGN(size), 0, 0);
    }
    return ptr;
}

void    ft_free_vg(void *ptr)
{
    ft_free(ptr);
    VALGRIND_FREELIKE_BLOCK(ptr, 0);
}

void    *ft_realloc_vg(void *ptr, size_t old_size, size_t new_size)
{
    void *new_ptr = ft_realloc(ptr, new_size);
    if (new_ptr == ptr) {
        VALGRIND_RESIZEINPLACE_BLOCK(ptr, old_size, new_size, 0);
    } else {
        if (ptr)
            VALGRIND_FREELIKE_BLOCK(ptr, 0);
        if (new_ptr)
            VALGRIND_MALLOCLIKE_BLOCK(new_ptr, new_size, 0, 0);
    }
    return new_ptr;
}

#endif