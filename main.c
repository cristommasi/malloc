#include "../include/malloc.h"
#include "./test_valgrind.h"
/* main.c */
#include "test_valgrind.h"
#include <string.h>

static void test_basic(void)
{
    char *a = ft_malloc_vg(64);
    assert(a);
    memset(a, 0xAA, 64);
    ft_free_vg(a);

    char *b = ft_malloc_vg(32);
    assert(b);
    memset(b, 0xBB, 32);
    b = ft_realloc_vg(b, 32, 128);
    assert(b);
    memset(b, 0xCC, 128);
    ft_free_vg(b);

    char *c = ft_malloc_vg(256);
    assert(c);
    memset(c, 0xDD, 256);
    c = ft_realloc_vg(c, 256, 16);
    assert(c);
    ft_free_vg(c);

    char *d = ft_realloc_vg(NULL, 0, 48);
    assert(d);
    memset(d, 0xEE, 48);
    ft_free_vg(d);

    char *e = ft_malloc_vg(64);
    assert(e);
    ft_realloc_vg(e, 64, 0);
}

static void test_mallopt(void)
{
    assert(ft_mallopt(_MALLOC_PERTURB_PARAM_, 0xCD) == 1);
    assert(ft_mallopt(_MALLOC_CHECK_PARAM_,   2)    == 1);
    assert(ft_mallopt(9999, 0) == 0);
}

static void test_show(void)
{
    ft_mallopt(_MALLOC_PERTURB_PARAM_, 55);
    char *x = ft_malloc_vg(64);




    char *y = ft_malloc_vg(64);




    printf("\n\n");
    show_alloc_mem_ex(1);
    // show_alloc_mem_ex(1);

    ft_free_vg(x);


    // show_alloc_mem();
    // show_alloc_mem_ex(0);
    // show_alloc_mem_ex(1);
}

static void test_fragmentation(void)
{
    /* 1. alternating free */
    void *frag[20];
    for (int i = 0; i < 20; i++)
        frag[i] = ft_malloc_vg(32);
    for (int i = 0; i < 20; i += 2)
    {
        ft_free_vg(frag[i]);
        frag[i] = NULL;
    }

    for (int i = 0; i < 20; i += 2)
    {
        frag[i] = ft_malloc_vg(32);
        assert(frag[i]);
    }
    for (int i = 0; i < 20; i++)
        ft_free_vg(frag[i]);

    /* 2. ascending size — neighbour corruption check */
    void *s[5];
    s[0] = ft_malloc_vg(16);
    s[1] = ft_malloc_vg(32);
    s[2] = ft_malloc_vg(64);
    s[3] = ft_malloc_vg(32);
    s[4] = ft_malloc_vg(16);
    memset(s[0], 0x1, 16);
    memset(s[2], 0x2, 64);
    memset(s[4], 0x3, 16);
    ft_free_vg(s[1]);
    ft_free_vg(s[3]);
    void *big = ft_malloc_vg(64);
    assert(big);
    assert(*(char*)s[0] != 0);
    assert(*(char*)s[2] != 0);
    assert(*(char*)s[4] != 0);
    ft_free_vg(s[0]);
    ft_free_vg(s[2]);
    ft_free_vg(s[4]);
    ft_free_vg(big);

    /* 3. coalescing */
    void *co[4];
    for (int i = 0; i < 4; i++)
        co[i] = ft_malloc_vg(32);
    ft_free_vg(co[1]);
    ft_free_vg(co[2]);
    void *merged = ft_malloc_vg(64);
    assert(merged);

    ft_free_vg(co[0]);
    ft_free_vg(co[3]);
    ft_free_vg(merged);

    /* 4. cross-type */
    void *t  = ft_malloc_vg(32);
    void *sm = ft_malloc_vg(512);
    void *lg = ft_malloc_vg(4096);
    ft_free_vg(sm);
    void *sm2 = ft_malloc_vg(256);
    assert(sm2);

    ft_free_vg(t);
    ft_free_vg(sm2);
    ft_free_vg(lg);

    /* 5. rapid churn */
    void *churn;
    for (int i = 0; i < 500; i++)
    {
        churn = ft_malloc_vg(48);
        assert(churn);
        memset(churn, (char)i, 48);
        ft_free_vg(churn);
    }
}

int main(void)
{
    test_basic();
    test_mallopt();
    test_show();
    test_fragmentation();
    return 0;
}