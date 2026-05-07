#include "./include/malloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>


#ifdef USE_SYSTEM
#  define MY_MALLOC  malloc
#  define MY_FREE    free
#  define MY_REALLOC realloc
#else
#  define MY_MALLOC  ft_malloc
#  define MY_FREE    ft_free
#  define MY_REALLOC ft_realloc
void *ft_malloc(size_t);
void  ft_free(void *);
void *ft_realloc(void *, size_t);
#endif


static int g_pass = 0, g_fail = 0;

#define CHECK(cond, msg) \
    do { \
        if (cond) { printf("  [PASS] %s\n", msg); g_pass++; } \
        else       { printf("  [FAIL] %s\n", msg); g_fail++; } \
    } while(0)

#define ALIGNED(p) (((uintptr_t)(p) & 7) == 0)
#define SECTION(s) printf("\n── %s ──\n", s)


static void test_malloc_basic(void)
{
    SECTION("malloc – basic");

    /* size 0 must return NULL (your impl) or a unique ptr (glibc) */
    void *z = MY_MALLOC(0);
    CHECK(z == NULL, "malloc(0) returns NULL");

    /* tiny – exactly at boundary */
    void *t = MY_MALLOC(112);
    CHECK(t != NULL,       "malloc(112) not NULL");
    CHECK(ALIGNED(t),     "malloc(112) 8-byte aligned");
    memset(t, 0xAA, 112);
    CHECK(*((char*)t) == (char)0xAA, "malloc(112) writable");
	MY_FREE(t); 
		
    /* small */
    void *s = MY_MALLOC(512);

    CHECK(s != NULL && ALIGNED(s), "malloc(512) not NULL + aligned");
    memset(s, 0xBB, 512);
    CHECK(*((char*)s + 511) == (char)0xBB, "malloc(512) last byte writable");
	MY_FREE(s); 

    /* large */
    void *l = MY_MALLOC(65536);
    CHECK(l != NULL && ALIGNED(l), "malloc(65536) not NULL + aligned");
    memset(l, 0xCC, 65536);
    CHECK(*((char*)l + 65535) == (char)0xCC, "malloc(65536) last byte writable");
	MY_FREE(l);

    /* no overlap: two adjacent tiny allocs must not share memory */
    void *a = MY_MALLOC(64);
    void *b = MY_MALLOC(64);
    CHECK(a != b, "two malloc(64) return different ptrs");
    memset(a, 0x11, 64);
    memset(b, 0x22, 64);
    CHECK(*((char*)a) == (char)0x11, "a not clobbered by b");
    MY_FREE(a);
	MY_FREE(b);
}

static void test_malloc_stress(void)
{
    SECTION("malloc – stress (128 tiny + 128 small + 8 large)");
    void *tiny[128], *small[128], *large[8];
    int ok = 1;

    for (int i = 0; i < 128; i++) {
        tiny[i] = MY_MALLOC(112);
        if (!tiny[i] || !ALIGNED(tiny[i])) { ok = 0; break; }
        memset(tiny[i], i & 0xFF, 112);
    }
    CHECK(ok, "128 x malloc(112) all succeed + aligned");

    ok = 1;
    for (int i = 0; i < 128; i++) {
        small[i] = MY_MALLOC(512);
        if (!small[i] || !ALIGNED(small[i])) { ok = 0; break; }
        memset(small[i], i & 0xFF, 512);
    }
    CHECK(ok, "128 x malloc(512) all succeed + aligned");

    ok = 1;
    for (int i = 0; i < 8; i++) {
        large[i] = MY_MALLOC(1024 * 64);
        if (!large[i] || !ALIGNED(large[i])) { ok = 0; break; }
    }
    CHECK(ok, "8 x malloc(64KB) all succeed + aligned");

    /* verify no cross-contamination after all allocs */
    ok = 1;
    for (int i = 0; i < 128; i++)
        if (*((unsigned char*)tiny[i]) != (i & 0xFF)) { ok = 0; break; }
    CHECK(ok, "tiny blocks not clobbered by small/large allocs");

    for (int i = 0; i < 128; i++) { MY_FREE(tiny[i]); MY_FREE(small[i]); }
    for (int i = 0; i < 8;   i++) MY_FREE(large[i]);
}

static void test_free(void)
{
    SECTION("free");

    /* free(NULL) must not crash */
    MY_FREE(NULL);
    CHECK(1, "free(NULL) does not crash");

    /* free then re-malloc same size should reuse fastbin/CIS slot */
    void *p = MY_MALLOC(64);
    MY_FREE(p);
    void *p2 = MY_MALLOC(64);
    CHECK(p2 != NULL, "malloc after free returns non-NULL");
    /* reuse is an optimization, not guaranteed — just check no crash */
    MY_FREE(p2);

    /* fill-free-realloc: freed memory must not alias live allocation */
    char *a = MY_MALLOC(96);
    char *b = MY_MALLOC(96);
    memset(a, 'A', 96);
    memset(b, 'B', 96);
    MY_FREE(a);
    char *c = MY_MALLOC(96);  /* may reuse a's slot */
    memset(c, 'C', 96);
    CHECK(b[0] == 'B', "b not clobbered after a freed and c allocated");
    MY_FREE(b); MY_FREE(c);

    /* free all 128 tiny blocks then re-malloc — heap must still work */
    void *arr[128];
    for (int i = 0; i < 128; i++) arr[i] = MY_MALLOC(80);
    for (int i = 0; i < 128; i++) MY_FREE(arr[i]);
    void *fresh = MY_MALLOC(80);
    CHECK(fresh != NULL, "malloc(80) after freeing 128 blocks succeeds");
    MY_FREE(fresh);
}

static void test_realloc(void)
{
    SECTION("realloc – corner cases");

    /* realloc(NULL, n) == malloc(n) */
    void *p = MY_REALLOC(NULL, 64);
    CHECK(p != NULL && ALIGNED(p), "realloc(NULL,64) == malloc(64)");
    MY_FREE(p);

    /* realloc(p, 0) == free(p) — returns NULL */
    void *q = MY_MALLOC(64);
    void *r = MY_REALLOC(q, 0);
    CHECK(r == NULL, "realloc(p,0) returns NULL");

    SECTION("realloc – grow");

    char *s = MY_MALLOC(64);
    memset(s, 'X', 64);
    char *s2 = MY_REALLOC(s, 512);
    CHECK(s2 != NULL,          "realloc 64→512 not NULL");
    CHECK(ALIGNED(s2),        "realloc 64→512 aligned");
    CHECK(s2[0] == 'X',       "realloc 64→512 preserved byte 0");
    CHECK(s2[63] == 'X',      "realloc 64→512 preserved byte 63");
    memset(s2, 'Y', 512);
    CHECK(s2[511] == 'Y',     "realloc 64→512 new region writable");

    /* grow across zone boundary: tiny → large */
    char *tl = MY_MALLOC(112);
    memset(tl, 'T', 112);
    char *tl2 = MY_REALLOC(tl, 8192);
    CHECK(tl2 != NULL,         "realloc 112→8192 not NULL");
    CHECK(tl2[0] == 'T',      "realloc 112→8192 preserved byte 0");
    CHECK(tl2[111] == 'T',    "realloc 112→8192 preserved byte 111");
    MY_FREE(tl2);

    SECTION("realloc – shrink");

    char *big = MY_MALLOC(1024);
    memset(big, 'Z', 1024);
    char *small2 = MY_REALLOC(big, 64);
    CHECK(small2 != NULL,      "realloc 1024→64 not NULL");
    CHECK(small2[0] == 'Z',   "realloc 1024→64 preserved byte 0");
    CHECK(small2[63] == 'Z',  "realloc 1024→64 preserved byte 63");
    MY_FREE(small2); MY_FREE(s2);

    SECTION("realloc – same size");
    char *same = MY_MALLOC(128);
    memset(same, 'S', 128);
    char *same2 = MY_REALLOC(same, 128);
    CHECK(same2 != NULL,       "realloc same size not NULL");
    CHECK(same2[0] == 'S',    "realloc same size data intact");
    MY_FREE(same2);
}


static void test_interleaved(void)
{
    SECTION("interleaved – alloc/free/realloc mix");
    #define N 64
    char *ptrs[N];
    int ok = 1;

    for (int i = 0; i < N; i++) {
		
        ptrs[i] = MY_MALLOC(16 + i * 8);  /* 16, 24, 32, … 520 */
        if (!ptrs[i]) { ok = 0; break; }
        ft_memset(ptrs[i], i, 16 + i * 8);
    }
    CHECK(ok, "64 varied allocs succeeded");
	
    /* free every other one */
    for (int i = 0; i < N; i += 2) { MY_FREE(ptrs[i]); ptrs[i] = NULL; }

    /* realloc survivors to a larger size */
    ok = 1;
    for (int i = 1; i < N; i += 2) {
        char expected = (char)i;
        ptrs[i] = MY_REALLOC(ptrs[i], (16 + i * 8) * 2);
        if (!ptrs[i] || ptrs[i][0] != expected) { ok = 0; break; }
    }
    CHECK(ok, "realloc survivors: data intact after interleaved free");

    /* fill the freed slots again */
    ok = 1;
    for (int i = 0; i < N; i += 2) {
        ptrs[i] = MY_MALLOC(16 + i * 8);
        if (!ptrs[i]) { ok = 0; break; }
        memset(ptrs[i], i | 0x80, 16 + i * 8);
    }
    CHECK(ok, "re-fill freed slots succeeded");

    /* final integrity: survivors must not have been clobbered */
    ok = 1;
    for (int i = 1; i < N; i += 2)
        if (ptrs[i][0] != (char)i) { ok = 0; break; }
    CHECK(ok, "survivors not clobbered by re-fill");

    for (int i = 0; i < N; i++) if (ptrs[i]) MY_FREE(ptrs[i]);
}


static void test_boundaries(void)
{
    SECTION("boundary sizes");
    size_t sizes[] = {
        1, 7, 8, 9,         /* sub-alignment */
        111, 112, 113,        /* around TINY_CHUNK_MAX */
        1007, 1008, 1009,    /* around SMALL_CHUNK_MAX */
        4096, 4097,           /* page boundary */
        65536                 /* large */
    };
    int n = sizeof(sizes) / sizeof(sizes[0]);
    int ok = 1;
    void *ptrs[16];

    for (int i = 0; i < n; i++) {
        ptrs[i] = MY_MALLOC(sizes[i]);
        if (!ptrs[i] || !ALIGNED(ptrs[i])) { ok = 0; break; }
        memset(ptrs[i], 0xDE, sizes[i]);
        if (*((unsigned char*)ptrs[i] + sizes[i] - 1) != 0xDE) { ok = 0; break; }
    }
    CHECK(ok, "all boundary sizes: allocated, aligned, writable");
    for (int i = 0; i < n; i++) MY_FREE(ptrs[i]);
}


int main(void) {

    
    printf("=== ft_malloc test suite ===\n");
    test_malloc_basic();

    test_malloc_stress();

    test_free();

    test_realloc();

    test_interleaved();

    test_boundaries();

    printf("\n=== results: %d passed, %d failed ===\n", g_pass, g_fail);
    return (g_fail ? 1 : 0);
}