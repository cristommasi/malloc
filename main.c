#include <unistd.h>
#include <stdlib.h>
#include "./include/malloc.h"

static void	print_str(const char *s)
{
	int i = 0;
	while (s[i])
		i++;
	write(1, s, i);
}

static void	print_num(size_t n)
{
	char	buf[20];
	int		i = 19;

	buf[i] = '\0';
	if (n == 0) { write(1, "0", 1); return ; }
	while (n > 0) { buf[--i] = '0' + (n % 10); n /= 10; }
	print_str(&buf[i]);
}

static void	print_ptr(void *p)
{
	unsigned long	n = (unsigned long)p;
	char			buf[18];
	int				i = 17;
	const char		*hex = "0123456789abcdef";

	buf[i] = '\0';
	if (n == 0) { print_str("(nil)"); return ; }
	while (n > 0) { buf[--i] = hex[n % 16]; n /= 16; }
	print_str("0x");
	print_str(&buf[i]);
}

static void	ok(const char *label)
{
	print_str("[OK] ");
	print_str(label);
	print_str("\n");
}

static void	fail(const char *label)
{
	print_str("[FAIL] ");
	print_str(label);
	print_str("\n");
}

static void	fill(char *p, char c, size_t n)
{
	for (size_t i = 0; i < n; i++)
		p[i] = c;
}

static int	check(char *p, char c, size_t n)
{
	for (size_t i = 0; i < n; i++)
		if (p[i] != c) return 0;
	return 1;
}

/* ------------------------------------------------------------------ */
/* split_center: shrink within small range (128–1008), stays in chunk  */
/* ------------------------------------------------------------------ */
static void	test_split_center(void)
{
	print_str("\n--- split_center (shrink, stay small) ---\n");

	char *p = malloc(800);
	if (!p) { fail("malloc 800"); return ; }
	fill(p, 'A', 800);

	char *p2 = realloc(p, 256);
	if (!p2) { fail("realloc 800->256"); return ; }
	if (!check(p2, 'A', 256)) { fail("data preserved after shrink"); return ; }
	ok("realloc 800->256 (split_center)");

	/* shrink again */
	char *p3 = realloc(p2, 128);
	if (!p3) { fail("realloc 256->128"); return ; }
	if (!check(p3, 'A', 128)) { fail("data preserved after second shrink"); return ; }
	ok("realloc 256->128 (split_center)");

	free(p3);
}

/* ------------------------------------------------------------------ */
/* split_right: grow, next chunk is free and large enough              */
/* ------------------------------------------------------------------ */
static void	test_split_right(void)
{
	print_str("\n--- split_right (grow into free next chunk) ---\n");

	/* allocate two adjacent small chunks, free the second */
	char *a = malloc(128);
	char *b = malloc(512);
	if (!a || !b) { fail("malloc setup"); return ; }
	fill(a, 'B', 128);
	free(b);

	/* now realloc a to consume b's freed space */
	char *a2 = realloc(a, 400);
	if (!a2) { fail("realloc 128->400"); return ; }
	if (!check(a2, 'B', 128)) { fail("data preserved after split_right"); return ; }
	ok("realloc 128->400 (split_right)");

	free(a2);
}

/* ------------------------------------------------------------------ */
/* split_cis: grow into contiguous free space at top of heap           */
/* ------------------------------------------------------------------ */
static void	test_split_cis(void)
{
	print_str("\n--- split_cis (grow into top-of-heap free space) ---\n");

	/* allocate at top of a fresh heap region, no next allocated chunk */
	char *p = malloc(200);
	if (!p) { fail("malloc 200"); return ; }
	fill(p, 'C', 200);

	/* grow — should use cis (wilderness) space at heap top */
	char *p2 = realloc(p, 600);
	if (!p2) { fail("realloc 200->600"); return ; }
	if (!check(p2, 'C', 200)) { fail("data preserved after split_cis"); return ; }
	ok("realloc 200->600 (split_cis)");

	free(p2);
}

/* ------------------------------------------------------------------ */
/* split_left: grow, previous chunk is free and large enough           */
/* ------------------------------------------------------------------ */
static void	test_split_left(void)
{
	print_str("\n--- split_left (grow into free prev chunk) ---\n");

	/* lay out: [free gap][target] so prev is free */
	char *gap  = malloc(400);
	char *tgt  = malloc(128);
	if (!gap || !tgt) { fail("malloc setup"); return ; }
	fill(tgt, 'D', 128);

	/* allocate something after tgt so realloc can't go right */
	char *blocker = malloc(128);
	if (!blocker) { fail("malloc blocker"); return ; }
	fill(blocker, 'Z', 128);

	free(gap);

	/* realloc tgt — right is blocked, cis unavailable, prev is free */
	char *tgt2 = realloc(tgt, 300);
	if (!tgt2) { fail("realloc 128->300 (split_left)"); free(blocker); return ; }
	if (!check(tgt2, 'D', 128)) { fail("data preserved after split_left"); free(blocker); return ; }
	ok("realloc 128->300 (split_left)");

	free(tgt2);
	free(blocker);
}

/* ------------------------------------------------------------------ */
/* fallback: no split possible -> new malloc+free                      */
/* ------------------------------------------------------------------ */
static void	test_fallback(void)
{
	print_str("\n--- fallback (no split possible, full remalloc) ---\n");

	char *a = malloc(128);
	char *b = malloc(128);
	char *c = malloc(128);
	if (!a || !b || !c) { fail("malloc setup"); return ; }
	fill(b, 'E', 128);

	/* b is surrounded on both sides, no cis — must remalloc */
	char *b2 = realloc(b, 900);
	if (!b2) { fail("realloc surrounded chunk 128->900"); free(a); free(c); return ; }
	if (!check(b2, 'E', 128)) { fail("data preserved after fallback realloc"); free(a); free(c); return ; }
	ok("realloc surrounded 128->900 (fallback)");

	print_str("  old ptr="); print_ptr(b);
	print_str(" new ptr="); print_ptr(b2); print_str("\n");

	free(a);
	free(b2);
	free(c);
}

/* ------------------------------------------------------------------ */
/* edge cases                                                           */
/* ------------------------------------------------------------------ */
static void	test_edges(void)
{
	print_str("\n--- edge cases ---\n");

	/* realloc(NULL, n) == malloc(n) */
	char *p = realloc(NULL, 256);
	if (!p) { fail("realloc(NULL, 256)"); }
	else { fill(p, 'F', 256); ok("realloc(NULL,256) acts as malloc"); free(p); }

	/* realloc(p, 0) == free(p) */
	char *q = malloc(256);
	if (!q) { fail("malloc for realloc(p,0)"); return ; }
	void *r = realloc(q, 0);
	if (r != NULL) free(r);
	ok("realloc(p, 0) acts as free");

	/* same size realloc */
	char *s = malloc(512);
	if (!s) { fail("malloc 512"); return ; }
	fill(s, 'G', 512);
	char *s2 = realloc(s, 512);
	if (!s2) { fail("realloc same size"); return ; }
	if (!check(s2, 'G', 512)) { fail("data preserved same-size realloc"); return ; }
	ok("realloc same size");
	free(s2);
}

int	main(void)
{
	print_str("=== realloc split tests (small chunks 128-1008) ===\n");

	test_split_center();
	show_alloc_mem();
	test_split_right();
	show_alloc_mem();
	test_split_cis();
	show_alloc_mem();
	test_split_left();
	show_alloc_mem();
	test_fallback();
	show_alloc_mem();
	test_edges();

	
	print_str("\n=== done ===\n");
	return 0;
}