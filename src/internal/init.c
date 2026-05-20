#include "../../include/malloc_internal.h"

t_arena g_arena = {

	.lock          = PTHREAD_MUTEX_INITIALIZER,
	.OPS           = 
	{
		.SHOW_INFO = M_SHOW_DEFAULT,
		.PERTURB   = M_PERTURB_NONE,
		.CHECK     = M_CHECK_DEFAULT,
		.ARENA_MAX = M_ARENA_MAX_DEFAULT,
	},
	.tiny          = NULL,
	.small         = NULL,
	.large         = NULL,
	.fastbin       = {0},
	.smallbin      = {0},
	.heap_count    = 0
};


CONSTRUCTOR static void malloc_ctor(void) {


    pthread_mutex_init(&g_arena.lock, NULL);

    g_arena.OPS.SHOW_INFO       = M_SHOW_DEFAULT;
    g_arena.OPS.PERTURB         = M_PERTURB_NONE;
    g_arena.OPS.CHECK           = M_CHECK_DEFAULT;
    g_arena.OPS.ARENA_MAX       = M_ARENA_MAX_DEFAULT;
    char *val = NULL;

	if ((val = getenv("MALLOC_CHECK")) != NULL) {

		mallopt_internal(MALLOC_CHECK, asciitoint(val));
	}
	if ((val = getenv("MALLOC_PERTURB")) != NULL) {

		mallopt_internal(MALLOC_PERTURB, asciitoint(val));
	}
	if ((val = getenv("MALLOC_ARENA_MAX")) != NULL) {

		mallopt_internal(MALLOC_ARENA_MAX, asciitoint(val));
	}
    if ((val = getenv("MALLOC_SHOW_INFO")) != NULL) {

		mallopt_internal(MALLOC_SHOW_INFO, asciitoint(val));
	}
}


DESTRUCTOR static void malloc_dtor(void) {

    pthread_mutex_destroy(&g_arena.lock);
}
