#include "../../include/malloc.h"

bool    has_perturb(void) {

    return ((bool)g_arena.OPS.PERTURB);
}

int     get_perturb_alloc(void) {

    return ((int)g_arena.OPS.PERTURB);
}

int     get_perturb_free(void) {

    return (~((int)g_arena.OPS.PERTURB));
}

uint8_t     get_check(void) {
    
    return (g_arena.OPS.CHECK);
}

bool    has_check(void) {

    return ((bool)g_arena.OPS.CHECK);
}

bool    has_arena_max(void) {

    return ((bool)g_arena.OPS.ARENA_MAX);
}

uint32_t    get_arena_max(void) {

    return (g_arena.OPS.ARENA_MAX);
}

bool    has_mmap_threshold(void) {

    return ((bool)g_arena.OPS.MMAP_THRESHOLD);
}

size_t  get_mmap_threshold(void) {

    return (g_arena.OPS.MMAP_THRESHOLD);
}

