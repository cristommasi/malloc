
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
	else if (param == M_ZERO && value == 0) {

		g_arena.OPS.ZERO = (uint8_t)0;
		return (M_PARAM_SUCCESS);

	}
	else if (param == M_SHOW_INFO && value >= 0) {

		g_arena.OPS.SHOW_INFO = (uint8_t)value;
		return (M_PARAM_SUCCESS);

	}
    return (M_PARAM_ERROR);
}
