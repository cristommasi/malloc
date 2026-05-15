#include "./include/malloc.h"

static void test_show(void)
{

    char *s1 = malloc(64);


    show_alloc_mem_ex();
    // mallopt(MALLOC_SHOW_INFO, 1);
    // char *x = malloc(129);
    // // char *y = malloc_vg(64);
    // // char *z = malloc_vg(64);
    
    // // free_vg(y);
    // show_alloc_mem_ex();
    // free(x);
    
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