#include "../include/malloc.h"


int		mallopt_internal(int param, int value) {

	if (param == _MALLOC_CHECK_PARAM_) {

		if (value >= 0 && value <= 3)
			g_arena.OPS.CHECK = (uint8_t)value;
		else
			return (M_PARAM_ERROR);
	}
	else if (param == _MALLOC_PERTURB_PARAM_) {

		if (value >= 0 && value <= 255)
			g_arena.OPS.PERTURB = (uint8_t)value;
		else
			return (M_PARAM_ERROR);
	}
	else if (param == _MALLOC_ARENA_MAX_PARAM_ && value >= 0) {

		if (value >= 0)
			g_arena.OPS.ARENA_MAX = (uint32_t)value;
		else
			return (M_PARAM_ERROR);
	}
	else if (param == _MALLOC_MMAP_THRESHOLD_PARAM_) {
		
		if (value >= 16)
			g_arena.OPS.MMAP_THRESHOLD = (size_t)value;
		else
			return (M_PARAM_ERROR);
	}
    return (0);
}
