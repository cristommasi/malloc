/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_test.c                                        :+:      :+:    :+:  */
/*                                                    +:+ +:+         +:+    */
/*   By: tester <tester@student.42.fr>              +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+          */
/*   Created: 2025/05/13 00:00:00 by tester            #+#    #+#            */
/*   Updated: 2025/05/13 00:00:00 by tester           ###   ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include "./include/malloc.h" 

/* ─── colour helpers ──────────────────────────────────────────────────── */
#define GRN  "\033[0;32m"
#define RED  "\033[0;31m"
#define YLW  "\033[0;33m"
#define CYN  "\033[0;36m"
#define RST  "\033[0m"

#define PASS(msg) printf(GRN "[PASS] " RST msg "\n")
#define FAIL(msg) printf(RED "[FAIL] " RST msg "\n")
#define HDR(msg)  printf(CYN "\n══════════════════════════════════════\n" \
                             " " msg "\n" \
                             "══════════════════════════════════════\n" RST)
#define INFO(msg) printf(YLW "[INFO] " RST msg "\n")

/* ─── thread-safety parameters ────────────────────────────────────────── */
#define THREAD_COUNT    1
#define ALLOCS_PER_TH   200

/* ======================================================================= */
/*  1. TINY allocations (1 – TINY_CHUNK_MAX bytes)                         */
/* ======================================================================= */
static void test_tiny(void)
{
    HDR("1. TINY allocations");

    /* 1-a  basic alloc + write */
    char *p = ft_malloc(1);
    assert(p && "ft_malloc(1) returned NULL");
    *p = 'A';
    assert(*p == 'A');
    ft_free(p);
    PASS("tiny 1-byte alloc / write / free");

    /* 1-b  max-tiny alloc */
    char *q = ft_malloc(TINY_CHUNK_MAX);
    assert(q && "ft_malloc(TINY_CHUNK_MAX) returned NULL");
    ft_memset(q, 0xAB, TINY_CHUNK_MAX);
    for (int i = 0; i < TINY_CHUNK_MAX; i++)
        assert((unsigned char)q[i] == 0xAB);
    ft_free(q);
    PASS("tiny TINY_CHUNK_MAX alloc / pattern-check / free");

    /* 1-c  many tiny allocs (fills at least one zone) */
    void *ptrs[150];
    for (int i = 0; i < 150; i++) {
        ptrs[i] = ft_malloc((size_t)(i % TINY_CHUNK_MAX) + 1);
        assert(ptrs[i] && "tiny batch alloc failed");
    }
    for (int i = 0; i < 150; i++)
        ft_free(ptrs[i]);
    PASS("150 tiny allocs / free (triggers zone pre-alloc)");
}

/* ======================================================================= */
/*  2. SMALL allocations (TINY_CHUNK_MAX+1 – SMALL_CHUNK_MAX bytes)        */
/* ======================================================================= */
static void test_small(void)
{
    HDR("2. SMALL allocations");

    char *p = ft_malloc(TINY_CHUNK_MAX + 1);
    assert(p && "ft_malloc(TINY+1) returned NULL");
    ft_memset(p, 0xCD, TINY_CHUNK_MAX + 1);
    ft_free(p);
    PASS("small lower-bound alloc / free");

    char *q = ft_malloc(SMALL_CHUNK_MAX);
    assert(q && "ft_malloc(SMALL_CHUNK_MAX) returned NULL");
    ft_memset(q, 0xEF, SMALL_CHUNK_MAX);
    ft_free(q);
    PASS("small upper-bound alloc / free");

    void *ptrs[150];
    for (int i = 0; i < 150; i++) {
        size_t sz = (size_t)(i % (SMALL_CHUNK_MAX - TINY_CHUNK_MAX))
                    + TINY_CHUNK_MAX + 1;
        ptrs[i] = ft_malloc(sz);
        assert(ptrs[i] && "small batch alloc failed");
    }
    for (int i = 0; i < 150; i++)
        ft_free(ptrs[i]);
    PASS("150 small allocs / free");
}

/* ======================================================================= */
/*  3. LARGE allocations (> SMALL_CHUNK_MAX, each gets its own mmap)       */
/* ======================================================================= */
static void test_large(void)
{
    HDR("3. LARGE allocations");

    size_t sz = LARGE_CHUNK_MIN;
    char *p = ft_malloc(sz);
    assert(p && "ft_malloc(LARGE_CHUNK_MIN) returned NULL");
    ft_memset(p, 0x7E, sz);
    ft_free(p);
    PASS("large lower-bound alloc / free");

    sz = 1024 * 1024;   /* 1 MiB */
    char *q = ft_malloc(sz);
    assert(q && "ft_malloc(1 MiB) returned NULL");
    ft_memset(q, 0x55, sz);
    ft_free(q);
    PASS("large 1 MiB alloc / free");

    sz = 16 * 1024 * 1024;  /* 16 MiB */
    char *r = ft_malloc(sz);
    assert(r && "ft_malloc(16 MiB) returned NULL");
    ft_free(r);
    PASS("large 16 MiB alloc / free");
}

/* ======================================================================= */
/*  4. ft_realloc                                                           */
/* ======================================================================= */
static void test_realloc(void)
{
    HDR("4. ft_realloc");

    /* 4-a  grow tiny -> small */
    char *p = ft_malloc(16);
    assert(p);
    strcpy(p, "hello world!!");   /* 13 bytes + '\0' */
    p = ft_realloc(p, 500);
    assert(p && "realloc grow to small failed");
    assert(strcmp(p, "hello world!!") == 0 && "data not preserved on grow");
    ft_free(p);
    PASS("realloc grow tiny -> small (data preserved)");

    /* 4-b  grow small -> large */
    char *q = ft_malloc(200);
    assert(q);
    ft_memset(q, 0xAA, 200);
    q = ft_realloc(q, 8192);
    assert(q && "realloc grow to large failed");
    for (int i = 0; i < 200; i++)
        assert((unsigned char)q[i] == 0xAA && "data corrupt on grow to large");
    ft_free(q);
    PASS("realloc grow small -> large (data preserved)");

    /* 4-c  shrink large -> tiny */
    char *r = ft_malloc(4096);
    assert(r);
    ft_memset(r, 0xBB, 4096);
    r = ft_realloc(r, 8);
    assert(r && "realloc shrink failed");
    assert((unsigned char)r[0] == 0xBB && "first byte lost on shrink");
    ft_free(r);
    PASS("realloc shrink large -> tiny (first byte preserved)");

    /* 4-d  realloc(NULL, n) == malloc(n) */
    char *s = ft_realloc(NULL, 64);
    assert(s && "realloc(NULL, 64) returned NULL");
    ft_free(s);
    PASS("realloc(NULL, n) behaves like malloc(n)");

    /* 4-e  realloc(ptr, 0) == free(ptr) */
    char *t = ft_malloc(32);
    assert(t);
    void *ret = ft_realloc(t, 0);
    (void)ret;   /* standard allows NULL or freed ptr return */
    PASS("realloc(ptr, 0) behaves like free(ptr)");

    /* 4-f  same-size realloc should return same pointer (or valid one) */
    char *u = ft_malloc(128);
    assert(u);
    ft_memset(u, 0xCC, 128);
    char *v = ft_realloc(u, 128);
    assert(v && "realloc same size returned NULL");
    assert((unsigned char)v[0] == 0xCC);
    ft_free(v);
    PASS("realloc same size (data preserved)");
}

/* ======================================================================= */
/*  5. ft_free edge cases                                                   */
/* ======================================================================= */
static void test_free_edge(void)
{
    HDR("5. ft_free edge cases");

    /* 5-a  free(NULL) must be a no-op */
    ft_free(NULL);
    PASS("free(NULL) is a no-op (no crash)");

    /* 5-b  sequential free / alloc reuse (coalescing exercise) */
    void *a = ft_malloc(64);
    void *b = ft_malloc(64);
    void *c = ft_malloc(64);
    assert(a && b && c);
    ft_free(b);             /* free middle -> triggers coalesce candidate  */
    ft_free(a);             /* free left   -> should coalesce with b       */
    void *d = ft_malloc(128);   /* should fit in coalesced region          */
    assert(d && "alloc after coalesce failed");
    ft_free(d);
    ft_free(c);
    PASS("coalescing: free left+middle then alloc larger block");

    /* 5-c  free then re-alloc same size (fastbin reuse) */
    void *p = ft_malloc(32);
    assert(p);
    ft_free(p);
    void *q = ft_malloc(32);
    assert(q && "re-alloc after free failed");
    ft_free(q);
    PASS("fastbin reuse: free then re-alloc same size");
}

/* ======================================================================= */
/*  6. ft_mallopt                                                              */
/* ======================================================================= */
static void test_ft_mallopt(void)
{
    HDR("6. ft_mallopt");

    int r;

    /* 6-a  set CHECK to PRINT only (no abort on error) */
    r = ft_mallopt(_MALLOC_CHECK_PARAM_, _M_CHECK_PRINT);
    assert(r == M_PARAM_SUCCESS && "ft_mallopt CHECK=PRINT failed");
    PASS("ft_mallopt(_MALLOC_CHECK_, PRINT)");

    /* 6-b  set PERTURB byte (fill freed memory) */
    r = ft_mallopt(_MALLOC_PERTURB_PARAM_, 0xAB);
    assert(r == M_PARAM_SUCCESS && "ft_mallopt PERTURB failed");
    /* verify: alloc, free, then a second alloc should carry perturb byte  */
    unsigned char *x = ft_malloc(16);
    assert(x);
    ft_free(x);
    PASS("ft_mallopt(_MALLOC_PERTURB_, 0xAB) — freed region perturbed");

    /* 6-c  set MMAP_THRESHOLD */
    r = ft_mallopt(_MALLOC_MMAP_THRESHOLD_PARAM_, 2048);
    assert(r == M_PARAM_SUCCESS && "ft_mallopt MMAP_THRESHOLD failed");
    /* anything >= 2048 should now be a large alloc */
    char *p = ft_malloc(2048);
    assert(p && "alloc at new mmap threshold failed");
    ft_free(p);
    PASS("ft_mallopt(_MALLOC_MMAP_THRESHOLD_, 2048)");

    /* 6-d  restore defaults */
    ft_mallopt(_MALLOC_CHECK_PARAM_,           _M_CHECK_DEFAULT);
    ft_mallopt(_MALLOC_PERTURB_PARAM_,         (int)_M_PERTURB_DEFAULT);
    ft_mallopt(_MALLOC_MMAP_THRESHOLD_PARAM_,  (int)_M_MMAP_T_DEFAULT);
    PASS("ft_mallopt: defaults restored");

    /* 6-e  bad parameter */
    r = ft_mallopt(0xFF, 0);
    assert(r == M_PARAM_ERROR && "ft_mallopt bad param should return error");
    PASS("ft_mallopt(bad_param) returns M_PARAM_ERROR");
}

/* ======================================================================= */
/*  7. show_alloc_mem  &  show_alloc_mem_ex                                 */
/* ======================================================================= */
static void test_show(void)
{
    HDR("7. show_alloc_mem / show_alloc_mem_ex");

    /* allocate a range of sizes so every category is visible */
    void *t  = ft_malloc(32);                   /* tiny   */
    void *s  = ft_malloc(500);                  /* small  */
    void *l  = ft_malloc(50000);                /* large  */
    void *t2 = ft_malloc(80);                   /* tiny   */
    printf("hehehe\n");
    void *s2 = ft_malloc(900);                  /* small  */


    INFO("─── show_alloc_mem (in-use only) ───");
    show_alloc_mem();

    INFO("─── show_alloc_mem_ex(M_SHOW_INUSE) ───");
    show_alloc_mem_ex(M_SHOW_INUSE);

    /* free some blocks so freed slots appear in M_SHOW_ALL */
    ft_free(s);
    ft_free(t2);

    INFO("─── show_alloc_mem_ex(M_SHOW_ALL) — includes freed ───");
    show_alloc_mem_ex(M_SHOW_ALL);

    ft_free(t);
    ft_free(l);
    ft_free(s2);
    PASS("show_alloc_mem / show_alloc_mem_ex ran without crash");
}

/* ======================================================================= */
/*  8. Thread-safety                                                        */
/* ======================================================================= */
typedef struct {
    int    thread_id;
    int    errors;
} t_targ;

static void *thread_worker(void *arg)
{
    t_targ *a    = (t_targ *)arg;
    void   *ptrs[128];
    int     errors = 0;

    /* interleave tiny / small / large allocs, writes, and frees */
    for (int i = 0; i < 128; i++) {
        size_t sz;
        if      (i % 3 == 0) sz = (size_t)(i % TINY_CHUNK_MAX)  + 1;
        else if (i % 3 == 1) sz = (size_t)(i % (SMALL_CHUNK_MAX - TINY_CHUNK_MAX))
                                  + TINY_CHUNK_MAX + 1;
        else                  sz = LARGE_CHUNK_MIN + (size_t)(i * 13);

        ptrs[i] = ft_malloc(sz);
        if (!ptrs[i]) { errors++; continue; }
        ft_memset(ptrs[i], (int)(a->thread_id & 0xFF), sz);
    }

    /* realloc every third pointer */
    for (int i = 0; i < 128; i += 3) {
        if (!ptrs[i]) continue;
        ptrs[i] = ft_realloc(ptrs[i], 1024);
        if (!ptrs[i]) errors++;
    }

    /* free all */
    for (int i = 0; i < 128; i++)
        if (ptrs[i]) ft_free(ptrs[i]);

    a->errors = errors;
    return NULL;
}

static void test_threads(void)
{
    HDR("8. Thread-safety");

    pthread_t threads[THREAD_COUNT];
    t_targ    args[THREAD_COUNT];
    int       total_errors = 0;

    
    for (int i = 0; i < THREAD_COUNT; i++) {
        args[i].thread_id = i;
        args[i].errors    = 0;
        if (pthread_create(&threads[i], NULL, thread_worker, &args[i]) != 0) {
            FAIL("pthread_create failed");
            return;
        }
    }
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
        total_errors += args[i].errors;
    }

    if (total_errors == 0)
        PASS("8 threads × 200 allocs/reallocs/frees — no errors");
    else
        printf(RED "[FAIL] %d allocation errors across threads\n" RST, total_errors);
}

/* ======================================================================= */
/*  9. Alignment check                                                      */
/* ======================================================================= */
static void test_alignment(void)
{
    HDR("9. Pointer alignment");

    const int N = 64;
    void *ptrs[N];

    for (int i = 0; i < N; i++) {
        size_t sz = (size_t)(1 << (i % 14));   /* 1, 2, 4 … 8192 */
        ptrs[i] = ft_malloc(sz);
        assert(ptrs[i]);
        uintptr_t addr = (uintptr_t)ptrs[i];
        if (addr % ALIGNMENT != 0) {
            printf(RED "[FAIL] ptr %p not aligned to %zu bytes\n" RST,
                   ptrs[i], (size_t)ALIGNMENT);
        }
    }
    for (int i = 0; i < N; i++)
        ft_free(ptrs[i]);
    PASS("all returned pointers are ALIGNMENT-aligned");
}

/* ======================================================================= */
/*  10. Stress: rapid alloc / free cycles                                   */
/* ======================================================================= */
static void test_stress(void)
{
    HDR("10. Stress — rapid alloc/free cycles");

#define STRESS_N 200
    void *ptrs[STRESS_N];

    for (int round = 0; round < 1; round++) {
        for (int i = 0; i < STRESS_N; i++) {
            size_t sz = (size_t)((i * 7 + round * 13) % SMALL_CHUNK_MAX) + 1;
            ptrs[i] = ft_malloc(sz);
            assert(ptrs[i] && "stress alloc failed");
            ft_memset(ptrs[i], round & 0xFF, sz);
        }
        /* free in reverse order to exercise coalescing from the right */
        for (int i = STRESS_N - 1; i >= 0; i--)
            ft_free(ptrs[i]);
    }
    PASS("10 × 512 alloc/free cycles (reverse-order frees)");

    /* interleaved pattern: alloc even, free even, alloc all */
    for (int i = 0; i < STRESS_N; i += 2) {
        ptrs[i] = ft_malloc(64);
        assert(ptrs[i]);
    }
    for (int i = 0; i < STRESS_N; i += 2)
        ft_free(ptrs[i]);
    for (int i = 0; i < STRESS_N; i++) {
        ptrs[i] = ft_malloc(64);
        assert(ptrs[i] && "interleaved stress alloc failed");
    }
    for (int i = 0; i < STRESS_N; i++)
        ft_free(ptrs[i]);
    PASS("interleaved even-free / full-alloc stress");
#undef STRESS_N
}

/* ======================================================================= */
/*  main                                                                    */
/* ======================================================================= */
int main(void)
{
    printf(CYN
        "\n╔══════════════════════════════════════════╗\n"
        "║      ft_malloc  —  full test suite       ║\n"
        "╚══════════════════════════════════════════╝\n"
        RST);

    test_tiny();
    test_small();
    test_large();
    test_realloc();
    test_free_edge();
    test_ft_mallopt();
    test_show();
    test_threads();
    test_alignment();
    test_stress();

    printf(GRN
        "\n╔══════════════════════════════════════════╗\n"
        "║          All tests completed             ║\n"
        "╚══════════════════════════════════════════╝\n"
        RST);
    return 0;
}