// #include "../include/malloc.h"



// #define BBLACK    "\033[30;1m"
// #define BRED      "\033[31;1m"
// #define BGREEN    "\033[32;1m"
// #define GREEN    "\033[32m"
// #define BYELLOW   "\033[33;1m"
// #define YELLOW   "\033[33m"
// #define BBLUE     "\033[34;1m"
// #define BLUE     "\033[34m"
// #define BMAGENTA  "\033[35;1m"
// #define MAGENTA  "\033[35m"
// #define BCYAN     "\033[36;1m"
// #define BWHITE    "\033[37;1m"
// #define BRESET    "\033[0m"

// #include <sys/time.h>
// #include <unistd.h>

// #define BAR_WIDTH 64
// #define SLEEP_MS(ms) usleep((ms) * 2000)

// // ─── which heap index owns this chunk ───────────────────────────────────────

// static int chunk_fastbin_idx(t_chunk *chunk) {
//     for (int i = 0; i < FASTBIN_COUNT; i++) {
//         t_chunk *cur = g_arena.fastbin[i];
//         while (cur) {
//             if (cur == chunk) return (i);
//             cur = cur->next;
//         }
//     }
//     return (-1);
// }

// // ─── single heap bar ────────────────────────────────────────────────────────

// static void print_heap_bar(t_heap *heap, const char *label) {
//     size_t total = heap->total_size;
//     char       state[BAR_WIDTH];
//     ft_memset(state, 'U', BAR_WIDTH);

//     // mark cis (unmapped/unused) region
//     size_t cis_offset = (char *)heap->free_cis_start - (char *)(heap + 1);
//     int cis_from = (int)((cis_offset * BAR_WIDTH) / total);
//     for (int i = cis_from; i < BAR_WIDTH; i++)
//         state[i] = '.';

//     // mark fastbin chunks
//     t_chunk *cur = heap_to_chunk(heap);
//     while ((void *)cur < (void *)heap->free_cis_start) {
//         size_t offset = (char *)cur - (char *)(heap + 1);
//         size_t csize  = sizeof(t_chunk) + cur->size;
//         int from = (int)((offset * BAR_WIDTH) / total);
//         int to   = (int)(((offset + csize) * BAR_WIDTH) / total);
//         if (to >= BAR_WIDTH) to = BAR_WIDTH - 1;
//         if (chunk_fastbin_idx(cur) >= 0)
//             for (int i = from; i <= to; i++)
//                 state[i] = 'F';
//         cur = (t_chunk *)((char *)cur + csize);
//     }

//     // print label + bar
//     printf("  %-12s [", label);
//     int i = 0;
//     while (i < BAR_WIDTH) {
//         char c = state[i];
//         int  j = i;
//         while (j < BAR_WIDTH && state[j] == c) j++;
//         if      (c == 'F') printf(BYELLOW);
//         else if (c == 'M') printf(BMAGENTA);
//         else if (c == '.') printf("\033[90m");   // dark grey = unmapped cis
//         else               printf(BMAGENTA);     // U = in use = magenta
//         for (int k = i; k < j; k++)
//             printf("%c", (c == '.') ? '-' : (c == 'F') ? 'F' : 'M');
//         printf(BRESET);
//         i = j;
//     }
//     size_t used = (char *)heap->free_cis_start - (char *)(heap + 1);
//     printf("] %zu/%zu\n", used, total);
// }

// // ─── fastbin chain bar ──────────────────────────────────────────────────────

// static void print_fastbin_chains(t_heap **heaps, int n_heaps) {
//     printf("\n  FASTBIN CHAINS:\n");
//     bool any = false;
//     for (int i = 0; i < FASTBIN_COUNT; i++) {
//         if (!g_arena.fastbin[i]) continue;
//         any = true;
//         printf("  [%3d] ", (i + 1) * FASTBIN_MIN_CHUNK);
//         t_chunk *cur = g_arena.fastbin[i];
//         while (cur) {
//             // find which heap this chunk belongs to
//             int hidx = -1;
//             for (int h = 0; h < n_heaps; h++) {
//                 if (heaps[h] && arena_owner_of_heap(heaps[h], cur)) {
//                     hidx = h;
//                     break;
//                 }
//             }
//             if (hidx >= 0)
//                 printf(BYELLOW "F%d" BRESET, hidx);
//             else
//                 printf(BYELLOW "F?" BRESET);
//             if (cur->next) printf(" -> ");
//             cur = cur->next;
//         }
//         printf(" -> NULL\n");
//     }
//     if (!any)
//         printf("  (empty)\n");
// }

// // ─── full arena snapshot ────────────────────────────────────────────────────

// static void print_arena_snapshot(const char *phase) {
//     // collect all heap pointers for fastbin chain labeling
//     t_heap *all_heaps[64];
//     int         n = 0;
//     for (t_heap *h = g_arena.tiny;  h; h = h->next) all_heaps[n++] = h;
//     for (t_heap *h = g_arena.small; h; h = h->next) all_heaps[n++] = h;
//     for (t_heap *h = g_arena.large; h; h = h->next) all_heaps[n++] = h;

//     printf("\033[2J\033[H");  // clear screen
//     printf(BCYAN "══════════════════════════════════════════════════════\n");
//     printf("  ARENA  —  %s\n", phase);
//     printf("══════════════════════════════════════════════════════\n" BRESET);

//     const char *groups[3]    = {"TINY", "SMALL", "LARGE"};
//     t_heap *heads[3]     = {g_arena.tiny, g_arena.small, g_arena.large};

//     for (int g = 0; g < 3; g++) {
//         if (!heads[g]) continue;
//         printf("\n  %s HEAPS:\n", groups[g]);
//         t_heap *cur = heads[g];
//         int idx = 0;
//         while (cur) {
//             char label[32];
//             snprintf(label, sizeof(label), "heap[%d]", idx++);
//             print_heap_bar(cur, label);
//             cur = cur->next;
//         }
//     }

//     print_fastbin_chains(all_heaps, n);
//     printf(BCYAN "══════════════════════════════════════════════════════\n" BRESET);
//     fflush(stdout);
// }

// // ─── main ───────────────────────────────────────────────────────────────────

// int main(void) {
//     srand(42);

//     #define MAX_ALLOCS 512
//     char   *ptrs[MAX_ALLOCS];
//     size_t  sizes[MAX_ALLOCS];
//     int     alive[MAX_ALLOCS];
//     int     n = 0;
//     char    buf[128];

//     ft_memset(ptrs,  0, sizeof(ptrs));
//     ft_memset(sizes, 0, sizeof(sizes));
//     ft_memset(alive, 0, sizeof(alive));

//     // tiny sizes: 8..128, small sizes: 136..1024
//     // mix them randomly so we get multiple heaps of each type
//     static const size_t size_pool[] = {
//         8, 8, 16, 16, 24, 32, 32, 48, 48, 64, 72, 80, 96, 112, 112, 128,   // tiny
//         136, 200, 256, 300, 400, 512, 512, 640, 768, 900, 1000, 1024,        // small
//     };
//     #define POOL_SIZE (int)(sizeof(size_pool) / sizeof(size_pool[0]))

//     // ── phase 1: random allocs until we have 3+ tiny and 2+ small heaps ──
//     print_arena_snapshot("start — empty");
//     SLEEP_MS(400);

//     for (int i = 0; i < 300 && n < MAX_ALLOCS; i++) {
//         size_t sz = size_pool[rand() % POOL_SIZE];
//         ptrs[n]   = ft_malloc(sz);
//         sizes[n]  = sz;
//         alive[n]  = 1;
//         if (ptrs[n]) {
//             snprintf(buf, sizeof(buf), "a%03d", n);
//             ft_strlcpy(ptrs[n], buf, sz < 4 ? sz : 4);
//         }
//         n++;
//         if (i % 12 == 11) {
//             snprintf(buf, sizeof(buf), "allocating... %d allocs", n);
//             print_arena_snapshot(buf);
//             SLEEP_MS(80);
//         }
//     }
//     print_arena_snapshot("phase 1 done — heaps filled");
//     SLEEP_MS(700);

//     // ── phase 2: free random ~40% → scatter fastbin across all heaps ──
//     int freed_count = 0;
//     for (int i = 0; i < n; i++) {
//         if (alive[i] && (rand() % 10) < 4) {
//             ft_free(ptrs[i]);
//             alive[i] = 0;
//             freed_count++;
//             if (freed_count % 8 == 0) {
//                 snprintf(buf, sizeof(buf), "random free... %d freed", freed_count);
//                 print_arena_snapshot(buf);
//                 SLEEP_MS(90);
//             }
//         }
//     }
//     print_arena_snapshot("phase 2 done — fastbins scattered");
//     SLEEP_MS(700);

//     // ── phase 3: reallocate random sizes from fastbin ──
//     int realloc_count = 0;
//     for (int i = 0; i < 80 && n < MAX_ALLOCS; i++) {
//         size_t sz = size_pool[rand() % POOL_SIZE];
//         ptrs[n]   = ft_malloc(sz);
//         sizes[n]  = sz;
//         alive[n]  = 1;
//         if (ptrs[n]) {
//             snprintf(buf, sizeof(buf), "r%03d", n);
//             ft_strlcpy(ptrs[n], buf, sz < 4 ? sz : 4);
//         }
//         n++;
//         realloc_count++;
//         if (realloc_count % 10 == 0) {
//             snprintf(buf, sizeof(buf), "reallocating... %d new", realloc_count);
//             print_arena_snapshot(buf);
//             SLEEP_MS(70);
//         }
//     }
//     print_arena_snapshot("phase 3 done — fastbin reused");
//     SLEEP_MS(700);

//     // ── phase 4: free in random bursts — watch heaps munmap one by one ──
//     // shuffle indices so deletion order is unpredictable
//     int order[MAX_ALLOCS];
//     for (int i = 0; i < n; i++) order[i] = i;
//     for (int i = n - 1; i > 0; i--) {
//         int j = rand() % (i + 1);
//         int tmp = order[i]; order[i] = order[j]; order[j] = tmp;
//     }

//     int phase4_freed = 0;
//     for (int i = 0; i < n; i++) {
//         int idx = order[i];
//         if (!alive[idx]) continue;
//         ft_free(ptrs[idx]);
//         alive[idx] = 0;
//         phase4_freed++;
//         // show more frequently early on so you see the first munmaps
//         int interval = (phase4_freed < 30) ? 4 : (phase4_freed < 80) ? 8 : 16;
//         if (phase4_freed % interval == 0) {
//             snprintf(buf, sizeof(buf), "draining... %d freed", phase4_freed);
//             print_arena_snapshot(buf);
//             SLEEP_MS(100);
//         }
//     }

//     print_arena_snapshot("phase 4 done — all freed");
//     SLEEP_MS(500);
//     print_arena_snapshot("final — should be empty");
//     return (0);
// }