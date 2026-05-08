#include "../include/malloc.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

// ─── color helpers ────────────────────────────────────────────────────────────
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define CYAN    "\033[0;36m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

static int g_pass = 0;
static int g_fail = 0;

static void pass(const char *label) {
    printf(GREEN "  [PASS]" RESET " %s\n", label);
    g_pass++;
}

static void fail(const char *label, const char *reason) {
    printf(RED "  [FAIL]" RESET " %s" YELLOW " → %s" RESET "\n", label, reason);
    g_fail++;
}

#define EXPECT_NOT_NULL(label, ptr) \
    ((ptr) ? pass(label) : fail(label, "expected non-NULL, got NULL"))

#define EXPECT_NULL(label, ptr) \
    (!(ptr) ? pass(label) : fail(label, "expected NULL, got non-NULL"))

#define EXPECT_EQ(label, a, b) \
    ((a) == (b) ? pass(label) : fail(label, "values not equal"))

#define EXPECT_STR(label, a, b) \
    (strcmp((a),(b)) == 0 ? pass(label) : fail(label, "strings differ"))

static void print_section(const char *name) {
    printf("\n" BOLD CYAN "══ %s ══" RESET "\n", name);
}

static void print_summary(void) {
    printf("\n" BOLD "══ Results: " GREEN "%d passed" RESET BOLD ", " RED "%d failed" RESET BOLD " ══" RESET "\n\n",
           g_pass, g_fail);
}


// ─── 1. BASIC MALLOC / FREE ───────────────────────────────────────────────────
static void test_basic_malloc_free(void) {
    print_section("Basic malloc / free");

    // malloc(0) must return a usable non-NULL pointer per your impl (size = 16)
    void *p0 = ft_malloc(0);
    EXPECT_NOT_NULL("malloc(0) non-NULL", p0);
    ft_free(p0);

    // Tiny allocation (≤ TINY_CHUNK_MAX = 144)
    void *p1 = ft_malloc(1);
    EXPECT_NOT_NULL("malloc(1) tiny", p1);
    *(char*)p1 = 'A';
    EXPECT_EQ("tiny write-back", *(char*)p1, 'A');
    ft_free(p1);

    void *p2 = ft_malloc(TINY_CHUNK_MAX);
    EXPECT_NOT_NULL("malloc(TINY_CHUNK_MAX=144)", p2);
    ft_free(p2);

    // Small allocation (145 – 1016)
    void *p3 = ft_malloc(145);
    EXPECT_NOT_NULL("malloc(145) small", p3);
    ft_free(p3);

    void *p4 = ft_malloc(SMALL_CHUNK_MAX);
    EXPECT_NOT_NULL("malloc(SMALL_CHUNK_MAX=1016)", p4);
    ft_free(p4);

    // Large allocation (≥ 1017)
    void *p5 = ft_malloc(LARGE_CHUNK_MIN);
    EXPECT_NOT_NULL("malloc(LARGE_CHUNK_MIN=1017) large", p5);
    ft_free(p5);

    void *p6 = ft_malloc(1 << 20);   // 1 MiB
    EXPECT_NOT_NULL("malloc(1 MiB)", p6);
    ft_free(p6);

    // free(NULL) must be a no-op
    ft_free(NULL);   // would crash / assert if broken
    pass("free(NULL) no-op");
}


// ─── 2. ALIGNMENT ─────────────────────────────────────────────────────────────
static void test_alignment(void) {
    print_section("Alignment (16-byte multiples)");

    size_t sizes[] = { 1, 3, 7, 8, 9, 15, 16, 17, 63, 64, 128, 256, 1024, 4096 };
    char   label[64];
    for (size_t i = 0; i < sizeof(sizes)/sizeof(sizes[0]); i++) {
        void *p = ft_malloc(sizes[i]);
        snprintf(label, sizeof(label), "malloc(%zu) aligned to 16", sizes[i]);
        if (p && ((uintptr_t)p % ALIGNMENT == 0))
            pass(label);
        else
            fail(label, "pointer not 16-byte aligned");
        ft_free(p);
    }
}


// ─── 3. FASTBIN RECYCLING ────────────────────────────────────────────────────
// After freeing a tiny chunk it goes to fastbin[idx].
// The very next malloc of the same size MUST return the same address.
static void test_fastbin_recycle(void) {
    print_section("Fastbin recycle (tiny re-use)");

    // sizes that map into fastbin (16 … 16+9*16 = 160, but tiny max is 144)
    size_t tiny_sizes[] = { 16, 32, 48, 64, 80, 96, 112, 128, 144 };
    char   label[64];

    for (size_t i = 0; i < sizeof(tiny_sizes)/sizeof(tiny_sizes[0]); i++) {
        void *p1 = ft_malloc(tiny_sizes[i]);
        ft_free(p1);
        void *p2 = ft_malloc(tiny_sizes[i]);
        snprintf(label, sizeof(label), "fastbin recycle size=%zu", tiny_sizes[i]);
        EXPECT_EQ(label, (uintptr_t)p1, (uintptr_t)p2);
        ft_free(p2);
    }
}


// ─── 4. SMALLBIN RECYCLING ───────────────────────────────────────────────────
static void test_smallbin_recycle(void) {

    print_section("Smallbin recycle (small re-use)");

    size_t small_sizes[] = { 160, 176, 256, 512, 1016 };
    char   label[64];

    for (size_t i = 0; i < sizeof(small_sizes)/sizeof(small_sizes[0]); i++) {
  
        void *anchor = ft_malloc(small_sizes[i]); // keeps heap alive (blocks >= 1)
        void *p1     = ft_malloc(small_sizes[i]);
        ft_free(p1);  
        void *p2     = ft_malloc(small_sizes[i]);
        snprintf(label, sizeof(label), "smallbin recycle size=%zu", small_sizes[i]);
        EXPECT_EQ(label, (uintptr_t)p1, (uintptr_t)p2);
        ft_free(p2);
        ft_free(anchor);
    }
}


// ─── 5. MULTIPLE LIVE ALLOCATIONS ────────────────────────────────────────────
#define BATCH 64
static void test_many_allocs(void) {
    print_section("Many simultaneous allocations");

    void *ptrs[BATCH];
    // Tiny batch
    for (int i = 0; i < BATCH; i++) {
        ptrs[i] = ft_malloc(8);
        *(int*)ptrs[i] = i;       // write unique value
    }
    int ok = 1;
    for (int i = 0; i < BATCH; i++) {
        if (*(int*)ptrs[i] != i) { ok = 0; break; }
    }
    ok ? pass("tiny batch: 64 allocs, data intact") : fail("tiny batch", "data corrupted");
    for (int i = 0; i < BATCH; i++)
        ft_free(ptrs[i]);

    // Small batch
    for (int i = 0; i < BATCH; i++) {
        ptrs[i] = ft_malloc(200);
        memset(ptrs[i], (unsigned char)i, 200);
    }
    ok = 1;
    for (int i = 0; i < BATCH; i++) {
        unsigned char *b = ptrs[i];
        for (int j = 0; j < 200 && ok; j++)
            if (b[j] != (unsigned char)i) ok = 0;
    }
    ok ? pass("small batch: 64 allocs, data intact") : fail("small batch", "data corrupted");
    for (int i = 0; i < BATCH; i++)
        ft_free(ptrs[i]);
}


// ─── 6. REALLOC – basic cases ─────────────────────────────────────────────────
static void test_realloc_basic(void) {
    print_section("Realloc – basic cases");

    // realloc(NULL, n)  ≡  malloc(n)
    void *p = ft_realloc(NULL, 64);
    EXPECT_NOT_NULL("realloc(NULL,64) == malloc(64)", p);
    ft_free(p);

    // realloc(ptr, 0)  ≡  free(ptr)
    p = ft_malloc(64);
    void *r = ft_realloc(p, 0);
    EXPECT_NULL("realloc(ptr,0) returns NULL", r);

    // same size → same pointer
    p = ft_malloc(64);
    r = ft_realloc(p, 64);
    EXPECT_EQ("realloc same size → same ptr", (uintptr_t)p, (uintptr_t)r);
    ft_free(r);
}


// ─── 7. REALLOC – grow / shrink within same type ────────────────────────────
static void test_realloc_inplace(void) {
    print_section("Realloc – grow / shrink (same heap type)");

    // grow tiny → still tiny
    void *p = ft_malloc(16);
    memset(p, 0xAB, 16);
    void *r = ft_realloc(p, 32);   // still ≤ 144 → same heap type
    EXPECT_NOT_NULL("realloc grow 16→32 (tiny)", r);
    // first 16 bytes must be preserved
    int ok = 1;
    for (int i = 0; i < 16 && ok; i++)
        if (((unsigned char*)r)[i] != 0xAB) ok = 0;
    ok ? pass("data preserved after grow") : fail("grow", "data not preserved");
    ft_free(r);

    // shrink small → still small
    p = ft_malloc(512);
    memset(p, 0xCD, 512);
    r = ft_realloc(p, 256);
    EXPECT_NOT_NULL("realloc shrink 512→256 (small)", r);
    ok = 1;
    for (int i = 0; i < 256 && ok; i++)
        if (((unsigned char*)r)[i] != 0xCD) ok = 0;
    ok ? pass("data preserved after shrink") : fail("shrink", "data not preserved");
    ft_free(r);
}


// ─── 8. REALLOC – cross-type (tiny → small → large) ─────────────────────────
static void test_realloc_cross_type(void) {
    print_section("Realloc – cross heap-type promotion");

    // tiny → small
    void *p = ft_malloc(64);
    memset(p, 0x11, 64);
    void *r = ft_realloc(p, 512);    // 512 > TINY_CHUNK_MAX → small
    EXPECT_NOT_NULL("realloc tiny→small", r);
    int ok = 1;
    for (int i = 0; i < 64 && ok; i++)
        if (((unsigned char*)r)[i] != 0x11) ok = 0;
    ok ? pass("data preserved tiny→small") : fail("tiny→small", "data not preserved");
    ft_free(r);

    // small → large
    p = ft_malloc(512);
    memset(p, 0x22, 512);
    r = ft_realloc(p, 2048);         // 2048 > SMALL_CHUNK_MAX → large
    EXPECT_NOT_NULL("realloc small→large", r);
    ok = 1;
    for (int i = 0; i < 512 && ok; i++)
        if (((unsigned char*)r)[i] != 0x22) ok = 0;
    ok ? pass("data preserved small→large") : fail("small→large", "data not preserved");
    ft_free(r);

    // tiny → large (skip middle tier)
    p = ft_malloc(32);
    memset(p, 0x33, 32);
    r = ft_realloc(p, 4096);
    EXPECT_NOT_NULL("realloc tiny→large", r);
    ok = 1;
    for (int i = 0; i < 32 && ok; i++)
        if (((unsigned char*)r)[i] != 0x33) ok = 0;
    ok ? pass("data preserved tiny→large") : fail("tiny→large", "data not preserved");
    ft_free(r);
}


// ─── 9. HEAP EXPANSION (trigger new heap allocation) ────────────────────────
// TINY_HEAP_SIZE / (CHUNK_INUSE_SIZE + TINY_CHUNK_MAX) ≈ 102 allocs
// Allocate 120 tiny chunks to force a second heap to be mapped.
static void test_heap_expansion(void) {
    print_section("Heap expansion (force second heap mmap)");

#define N_EXPAND 120
    void *ptrs[N_EXPAND];
    int ok = 1;
    for (int i = 0; i < N_EXPAND; i++) {
        ptrs[i] = ft_malloc(TINY_CHUNK_MAX);   // 144 bytes each
        if (!ptrs[i]) { ok = 0; break; }
        *(int*)ptrs[i] = i;
    }
    ok ? pass("120 tiny allocs succeeded (forced heap expansion)") :
         fail("heap expansion", "allocation returned NULL");

    // verify all data still valid
    for (int i = 0; i < N_EXPAND && ok; i++)
        if (*(int*)ptrs[i] != i) { ok = 0; break; }
    ok ? pass("data intact across two tiny heaps") :
         fail("heap expansion data", "corruption detected");

    for (int i = 0; i < N_EXPAND; i++)
        ft_free(ptrs[i]);
}


// ─── 10. LARGE ALLOC ─────────────────────────────────────────────────────────
static void test_large_alloc(void) {
    print_section("Large allocations (individual mmap)");

    size_t large_sizes[] = { LARGE_CHUNK_MIN, 4096, 65536, 1<<20, 1<<22 };
    char   label[64];

    for (size_t i = 0; i < sizeof(large_sizes)/sizeof(large_sizes[0]); i++) {
        void *p = ft_malloc(large_sizes[i]);
        snprintf(label, sizeof(label), "large malloc(%zu)", large_sizes[i]);
        EXPECT_NOT_NULL(label, p);
        if (p) {
            memset(p, 0xFF, large_sizes[i]);   // touch all pages
            pass("large alloc: all pages writable");
            ft_free(p);
        }
    }
}


// ─── 11. SHOW_ALLOC_MEM smoke-test ───────────────────────────────────────────
static void test_show_alloc(void) {
    print_section("show_alloc_mem smoke-test");

    void *a = ft_malloc(8);
    void *b = ft_malloc(200);
    void *c = ft_malloc(2000);

    printf("  --- show_alloc_mem output begin ---\n");
    show_alloc_mem();
    printf("  --- show_alloc_mem output end   ---\n");
    pass("show_alloc_mem ran without crash");

    ft_free(a);
    ft_free(b);
    ft_free(c);
}


// ─── 12. MALLOPT ─────────────────────────────────────────────────────────────
static void test_mallopt(void) {
    print_section("ft_mallopt()");

    // Set perturb byte (0x55 = 0101 0101)
    int ret = ft_mallopt(_MALLOC_PERTURB_PARAM_, 0x55);
    EXPECT_EQ("mallopt(PERTURB, 0x55) returns 1", ret, 1);

    void *p = ft_malloc(64);
    EXPECT_NOT_NULL("alloc after perturb set", p);
    ft_free(p);

    // Reset perturb
    ft_mallopt(_MALLOC_PERTURB_PARAM_, 0);

    // Bad param
    ret = ft_mallopt(99, 0);
    EXPECT_EQ("mallopt(bad_param) returns 0", ret, 0);
}


// ─── 13. STRESS – interleaved malloc / free / realloc ────────────────────────
#define STRESS_N 256
static void test_stress(void) {
    print_section("Stress – interleaved alloc/free/realloc");

    void   *ptrs[STRESS_N] = {0};
    size_t  szs[STRESS_N]  = {0};

    // seed-free deterministic pseudo-random via LCG
    uint64_t rng = 0xDEADBEEFCAFEBABE;
#define NEXT_RNG() (rng = rng * 6364136223846793005ULL + 1442695040888963407ULL)

    for (int iter = 0; iter < 1024; iter++) {
        int    idx  = (int)(NEXT_RNG() % STRESS_N);
        size_t sz   = (size_t)(NEXT_RNG() % 2048) + 1;
        int    op   = (int)(NEXT_RNG() % 3);    // 0=malloc, 1=free, 2=realloc

        if (op == 0 || ptrs[idx] == NULL) {
            // malloc
            if (ptrs[idx]) ft_free(ptrs[idx]);
            ptrs[idx] = ft_malloc(sz);
            if (ptrs[idx]) { szs[idx] = sz; memset(ptrs[idx], (int)sz & 0xFF, sz); }
        }
        else if (op == 1) {
            // free
            ft_free(ptrs[idx]);
            ptrs[idx] = NULL; szs[idx] = 0;
        }
        else {
            // realloc
            void *nr = ft_realloc(ptrs[idx], sz);
            if (nr) { ptrs[idx] = nr; szs[idx] = sz; }
        }
    }

    // clean up survivors
    for (int i = 0; i < STRESS_N; i++)
        if (ptrs[i]) ft_free(ptrs[i]);

    pass("stress test completed without crash");
}


// ─── MAIN ─────────────────────────────────────────────────────────────────────
int main(void) {

    // test_basic_malloc_free();
    // test_alignment();
    // test_fastbin_recycle();
    // test_smallbin_recycle();
    // test_many_allocs();
    // test_realloc_basic();
    // test_realloc_inplace();
    test_realloc_cross_type();
    test_heap_expansion();
    test_large_alloc();
    test_show_alloc();
    test_mallopt();
    test_stress();

    print_summary();
    return (g_fail > 0) ? 1 : 0;
}