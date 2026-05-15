
#include "../../include/malloc_internal.h"

int		mallopt_internal(int param, int value) {

	if (param == MALLOC_CHECK && value >= 0 && value <= 3) {

		g_arena.OPS.CHECK = (uint8_t)value;
		return (M_PARAM_SUCCESS);
	}
	else if (param == MALLOC_PERTURB && value >= 0 && value <= 255) {

		g_arena.OPS.PERTURB = (uint8_t)value;
		return (M_PARAM_SUCCESS);

	}
	else if (param == MALLOC_ARENA_MAX && value >= 0) {

		g_arena.OPS.ARENA_MAX = (uint32_t)value;
		return (M_PARAM_SUCCESS);

	}
	else if (param == MALLOC_SHOW_INFO && value >= 0) {

		g_arena.OPS.SHOW_INFO = (uint8_t)value;
		return (M_PARAM_SUCCESS);

	}
    return (M_PARAM_ERROR);
}
