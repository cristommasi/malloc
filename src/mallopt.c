#include "../include/malloc.h"


int		mallopt_internal(int param, int value) {

	if (param == _MALLOC_CHECK_PARAM_) {

		g_arena.OPS.CHECK = (uint8_t)value;
	}
	else if (param == _MALLOC_PERTURB_PARAM_) {

		g_arena.OPS.PERTURB = (uint8_t)value;
	}
	else if (param == _MALLOC_ARENA_MAX_PARAM_) {

		g_arena.OPS.ARENA_MAX = (uint32_t)value;
	}
	else if (param == _MALLOC_MMAP_THRESHOLD_PARAM_) {
		
		g_arena.OPS.MMAP_THRESHOLD = (size_t)value;
	}
    //FOR NOW
    return (0);
}
