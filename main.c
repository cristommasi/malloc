#include "../include/malloc.h"


static void test_show(void)
{
    ft_mallopt(_MALLOC_PERTURB_PARAM_, 55);
    char *x = ft_malloc(64);
    // char *y = ft_malloc_vg(64);
    // char *z = ft_malloc_vg(64);
    
    ft_free(x);
    // ft_free_vg(y);
    // show_alloc_mem_ex(1);

    // show_alloc_mem();
    // show_alloc_mem_ex(0);
    // show_alloc_mem_ex(1);
}


int main(void)
{
    // test_basic();
    // test_mallopt();
    test_show();
    // test_fragmentation();
    return 0;
}