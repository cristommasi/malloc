
#include "./malloc_internal.h"

t_arena g_arena = {

	.lock          = PTHREAD_MUTEX_INITIALIZER,
	.OPS           = 
	{
		.SHOW_INFO = M_SHOW_DEFAULT,
		.PERTURB   = M_PERTURB_NONE,
		.CHECK     = M_CHECK_PRINT_ABORT,
		.ZERO      = M_ZERO_DEFAULT,
	},
	.tiny          = NULL,
	.small         = NULL,
	.large         = NULL,
	.fastbin       = {0},
	.smallbin      = {0},
	.tiny_cache	   = NULL,
	.small_cache   = NULL,
};


CONSTRUCTOR static void malloc_ctor(void) {


    pthread_mutex_init(&g_arena.lock, NULL);

    char *val = NULL;

	if ((val = getenv("MALLOC_CHECK_")) != NULL) {

		mallopt_internal(M_CHECK_ACTION, asciitoint(val));
	}
	if ((val = getenv("MALLOC_PERTURB_")) != NULL) {

		mallopt_internal(M_PERTURB, asciitoint(val));
	}
	if ((val = getenv("MALLOC_ZERO_")) != NULL) {

		mallopt_internal(M_ZERO, asciitoint(val));
	}
    if ((val = getenv("MALLOC_SHOW_")) != NULL) {

		mallopt_internal(M_SHOW_INFO, asciitoint(val));
	}
}


DESTRUCTOR static void malloc_dtor(void) {

    if (g_arena.tiny_cache != NULL) {

        arena_heap_munmap(g_arena.tiny_cache);
        g_arena.tiny_cache = NULL;
    }
    if (g_arena.small_cache != NULL) {
		
        arena_heap_munmap(g_arena.small_cache);
        g_arena.small_cache = NULL;
    }
    pthread_mutex_destroy(&g_arena.lock);
}
