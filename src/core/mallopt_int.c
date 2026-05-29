
#include "../malloc_internal.h"

int		mallopt_internal(int param, int value) {

	if (param == M_CHECK_ACTION && value >= 0 && value <= 3) {

		g_arena.OPS.CHECK = (uint8_t)value;
		return (M_PARAM_SUCCESS);
	}
	else if (param == M_PERTURB && value >= 0 && value <= 255) {

		g_arena.OPS.PERTURB = (uint8_t)value;
		return (M_PARAM_SUCCESS);

	}
	else if (param == M_ZERO && value >= 0) {

		if (value)
			g_arena.OPS.SHOW_INFO = (uint8_t)M_ZERO_INIT;
		else
			g_arena.OPS.SHOW_INFO = (uint8_t)M_ZERO_NONE;
		g_arena.OPS.ZERO = (uint8_t)value;
		return (M_PARAM_SUCCESS);

	}
	else if (param == M_SHOW_INFO && value >= 0) {

		if (value)
			g_arena.OPS.SHOW_INFO = (uint8_t)M_SHOW_ALL;
		else
			g_arena.OPS.SHOW_INFO = (uint8_t)M_SHOW_INUSE;
		return (M_PARAM_SUCCESS);

	}
    return (M_PARAM_ERROR);
}
