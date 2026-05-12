#include "../include/malloc.h"
#include <stdio.h>
#include <string.h>

static int passed = 0;
static int failed = 0;

#define EXPECT(label, cond) do { \
    if (cond) { printf("  [PASS] %s\n", label); passed++; } \
    else       { printf("  [FAIL] %s\n", label); failed++; } \
} while(0)

static void test_coalesce(void)
{
    printf("\n══ Coalesce – small chunks ══\n");

    /* 1. free middle → no coalesce (both neighbors in use) */
    {
        char *anchor = ft_malloc(160); /* prevents munmap */
        char *a = ft_malloc(160);
        char *b = ft_malloc(160);
        char *c = ft_malloc(160);
        ft_free(b);
        char *b2 = ft_malloc(160);
        EXPECT("free middle, realloc same size reuses slot", b2 == b);
        ft_free(a); ft_free(b2); ft_free(c); ft_free(anchor);
    }

    /* 2. free b then a → backward coalesce: a+b = 336 */
    {
        char *anchor = ft_malloc(160);
        char *a = ft_malloc(160);
        char *b = ft_malloc(160);
        char *c = ft_malloc(160);
        ft_free(b);
        ft_free(a);
        char *big = ft_malloc(336);
        EXPECT("backward coalesce: merged chunk reused for 336", big == a);
        ft_free(big); ft_free(c); ft_free(anchor);
    }

    /* 3. free b then c → forward coalesce: b+c = 336 */
    {
        char *anchor = ft_malloc(160);
        char *a = ft_malloc(160);
        char *b = ft_malloc(160);
        char *c = ft_malloc(160);
        ft_free(b);
        ft_free(c);
        char *big = ft_malloc(336);
        EXPECT("forward coalesce: merged chunk reused for 336", big == b);
        ft_free(a); ft_free(big); ft_free(anchor);
    }

    /* 4. free all three → full coalesce into 512 */
    {
        char *anchor = ft_malloc(160);
        char *a = ft_malloc(160);
        char *b = ft_malloc(160);
        char *c = ft_malloc(160);
        ft_free(b);
        ft_free(c);
        ft_free(a);
        char *big = ft_malloc(512);
        EXPECT("full coalesce: all three merged into 512", big == a);
        ft_free(big); ft_free(anchor);
    }

    /* 5. coalesce into CIS: freed chunk adjacent to CIS merges back */
    {
        char *anchor = ft_malloc(160);
        char *a = ft_malloc(160);
        char *b = ft_malloc(160);
        ft_free(b);
        ft_free(a);
        char *big = ft_malloc(800);
        EXPECT("coalesce into CIS: large alloc after free", big != NULL);
        if (big) ft_free(big);
        ft_free(anchor);
    }

    /* 6. write-back after coalesce reuse */
    {
        char *anchor = ft_malloc(160);
        char *a = ft_malloc(160);
        char *b = ft_malloc(160);
        ft_free(a);
        ft_free(b);
        char *big = ft_malloc(336);
        EXPECT("coalesced chunk is writable", big != NULL);
        if (big) {
            ft_memset(big, 0xAB, 336);
            EXPECT("coalesced chunk write-back correct",
                ((unsigned char*)big)[0]   == 0xAB &&
                ((unsigned char*)big)[335] == 0xAB);
            ft_free(big);
        }
        ft_free(anchor);
    }

    /* 7. mixed sizes: 160 + 320 backward coalesce = 496 */
    {
        char *anchor = ft_malloc(160);
        char *a = ft_malloc(160);
        char *b = ft_malloc(320);
        char *c = ft_malloc(160);
        ft_free(a);
        ft_free(b);
        char *big = ft_malloc(496);
        EXPECT("mixed size backward coalesce 160+320=496", big == a);
        ft_free(big); ft_free(c); ft_free(anchor);
    }

    printf("\n%d passed, %d failed\n", passed, failed);
}

int main(void)
{
    test_coalesce();
    return (failed > 0 ? 1 : 0);
}