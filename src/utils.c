#include "../include/malloc.h"



#define BBLACK    "\033[30;1m"
#define BRED      "\033[31;1m"
#define BGREEN    "\033[32;1m"
#define GREEN    "\033[32m"
#define BYELLOW   "\033[33;1m"
#define YELLOW   "\033[33m"
#define BBLUE     "\033[34;1m"
#define BLUE     "\033[34m"
#define BMAGENTA  "\033[35;1m"
#define MAGENTA  "\033[35m"
#define BCYAN     "\033[36;1m"
#define BWHITE    "\033[37;1m"
#define BRESET    "\033[0m"

// ─── helpers ────────────────────────────────────────────────────────────────


static bool chunk_in_fastbin(t_chunk *chunk) {
    for (int i = 0; i < FASTBIN_COUNT; i++) {
        t_chunk *cur = g_arena.fastbin[i];
        while (cur) {
            if (cur == chunk) return (true);
            cur = cur->next;
        }
    }
    return (false);
}

// ─── fastbin ────────────────────────────────────────────────────────────────

static void print_fastbin(void) {
    printf(BYELLOW"\n");
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║                    FASTBIN                       ║\n");
    printf("╚══════════════════════════════════════════════════╝\n"BRESET);
    bool any = false;
    for (int i = 0; i < FASTBIN_COUNT; i++) {
        if (!g_arena.fastbin[i]) continue;
        any = true;
        printf(BYELLOW"  [%3d bytes] "BRESET, ((i + 1) * 8) + 8);
        t_chunk *cur = g_arena.fastbin[i];
        while (cur) {
            printf("(%p)", cur);
            if (cur->next) printf(" -> ");
            cur = cur->next;
        }
        printf(MAGENTA" -> NULL\n"BRESET);
    }
    if (!any)
        printf("  (empty)\n");
}

// ─── heaps ──────────────────────────────────────────────────────────────────

static void print_heap_group(t_heap *head, const char *name) {
    if (!head) {
        return ;
    }
    t_heap *cur_heap = head;
    int heap_idx = 0;
    while (cur_heap) {
        printf(BMAGENTA"\n");
        printf("  ╔══════════════════════════════════════════════════╗\n");
        printf("  ║  %s HEAP [%d]  addr=%-16p           ║\n", name, heap_idx, cur_heap);
        printf("  ║  total_size=%-6zu  blocks=%-6zu                ║\n",cur_heap->total_size, cur_heap->blocks);
        printf("  ╚══════════════════════════════════════════════════╝\n"BRESET);

        t_chunk *cur  = heap_to_chunk(cur_heap);
        void        *end  = (char *)cur_heap + sizeof(t_heap) + cur_heap->total_size;
        int          bidx = 0;

        while ((void *)cur < end) {
            // stop at uninitialized cis memory
            if ((void *)cur >= (void *)cur_heap->free_cis_start)
                break ;
            bool freed = chunk_in_fastbin(cur);
            if (freed)
                printf("    [%d] " YELLOW "FREE " BRESET
                       "addr=%-16p size=%zu\n", bidx, cur, cur->size);
            else
                printf("    [%d] " BMAGENTA "USED " BRESET
                       "addr=%-16p size=%-6zu", bidx, cur, cur->size);
            if (!freed) {
                char *data = chunk_to_data(cur);
                printf(" data=\"");
                for (size_t i = 0; i < cur->size && data[i]; i++)
                    printf("%c", data[i]);
                printf("\"");
            }
            printf("\n");
            bidx++;
            cur = (t_chunk *)((char *)cur + sizeof(t_chunk) + cur->size);
        }
        heap_idx++;
        cur_heap = cur_heap->next;
    }
}

// ─── entry ──────────────────────────────────────────────────────────────────

void printALL(void) {
    printf(GREEN"\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║                      ARENA STATE                     ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n"BRESET);
    print_heap_group(g_arena.tiny,  "TINY ");
    print_heap_group(g_arena.small, "SMALL");
    print_heap_group(g_arena.large, "LARGE");
    print_fastbin();
    printf("\n");
}