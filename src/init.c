#include "../include/malloc_internal.h"

pthread_mutex_t	g_lock = PTHREAD_MUTEX_INITIALIZER;
t_arena         g_arena = {0};


CONSTRUCTOR static void malloc_ctor(void) {


    pthread_mutex_init(&g_lock, NULL);

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

    pthread_mutex_destroy(&g_lock);
}


PUBLIC void	*malloc(size_t size) {

	pthread_mutex_lock(&g_lock);
	void *ptr = malloc_internal(size);

    if (ptr != NULL) {

        VALGRIND_MALLOCLIKE_BLOCK(ptr, ALIGN(size), 0, 0);
    }

	pthread_mutex_unlock(&g_lock);

	return (ptr);
}

PUBLIC void	free(void *ptr) {

	pthread_mutex_lock(&g_lock);

	int err = free_internal(ptr);
    if (err == F_NO_ERROR) {

        if (ptr != NULL) {

            VALGRIND_FREELIKE_BLOCK(ptr, 0);
        }
        pthread_mutex_unlock(&g_lock);
        return ;
    }
    const char *msg = (err == F_MUMMAP_ERROR) ? F_MUNMAP_MSG : (err == F_DOUBLE_FREE_ERROR) ? F_DOUBLE_FREE_MSG : F_INV_PTR_MSG;
    uint8_t     check = get_check();
    
    if (check == M_CHECK_PRINT || check == M_CHECK_DEFAULT) {

        write(STDERR_FILENO, msg, 41);
    }
    if (check == M_CHECK_ABORT || check == M_CHECK_DEFAULT) {

        pthread_mutex_unlock(&g_lock);
        abort();
    }
    pthread_mutex_unlock(&g_lock);
}

PUBLIC void	*realloc(void *ptr, size_t size) {

    pthread_mutex_lock(&g_lock);

    size_t old_size = 0;
    if (ptr != NULL) {

        t_chunk *chunk = data_to_chunk(ptr);
        if (chunk != NULL) {

            old_size = get_size(chunk);
        }
    }

    void    *new_ptr = realloc_internal(ptr, size);
    size_t  new_size = ALIGN(size);

    if (new_ptr && new_ptr == ptr) {
        
        VALGRIND_RESIZEINPLACE_BLOCK(ptr, old_size, new_size, 0);
    }
    else {

        if (ptr && old_size) {

            VALGRIND_FREELIKE_BLOCK(ptr, 0);
        }
        if (new_ptr) {

            VALGRIND_MALLOCLIKE_BLOCK(new_ptr, new_size, 0, 0);
        }
    }

    pthread_mutex_unlock(&g_lock);

    return (new_ptr);
}

PUBLIC void    show_alloc_mem(void) {

    pthread_mutex_lock(&g_lock);

    show_alloc_mem_internal();

    pthread_mutex_unlock(&g_lock);
}


PUBLIC void    show_alloc_mem_ex() {

    pthread_mutex_lock(&g_lock);

    show_alloc_mem_ex_internal();

    pthread_mutex_unlock(&g_lock);
}

PUBLIC int     mallopt(int param, int value) {

    pthread_mutex_lock(&g_lock);

    int ret = mallopt_internal(param, value);
    
    pthread_mutex_unlock(&g_lock);
    
    return (ret);
}