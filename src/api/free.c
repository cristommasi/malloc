
#include "../malloc_internal.h"

PUBLIC void		free(void *ptr) {

	pthread_mutex_lock(&g_arena.lock);

    t_chunk *chunk = NULL;

    if (ptr == NULL) {
        pthread_mutex_unlock(&g_arena.lock);
        return ;
    }
    if ((uintptr_t)ptr % ALIGNMENT != 0) {
		
        arena_error_exit(F_INV_PTR_ERROR);
        pthread_mutex_unlock(&g_arena.lock);
        return ;
	}
    if ((chunk = data_to_chunk(ptr)) == NULL) {

        arena_error_exit(F_INV_PTR_ERROR);
        pthread_mutex_unlock(&g_arena.lock);
        return ;
	}

	int err = free_internal(chunk);


    if (err == F_NO_ERROR) {

        VALGRIND_FREELIKE_BLOCK(ptr, 0);
        pthread_mutex_unlock(&g_arena.lock);
        return ;
    }
    arena_error_exit(err);
    pthread_mutex_unlock(&g_arena.lock);
}