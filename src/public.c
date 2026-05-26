#include "../include/malloc_internal.h"


PUBLIC void	*malloc(size_t size) {

	pthread_mutex_lock(&g_arena.lock);
	void *ptr = malloc_internal(size);

    if (ptr != NULL) {

        VALGRIND_MALLOCLIKE_BLOCK(ptr, ALIGN(size), 0, 0);
    }

	pthread_mutex_unlock(&g_arena.lock);

	return (ptr);
}

PUBLIC void	free(void *ptr) {

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

PUBLIC void	*realloc(void *ptr, size_t size) {

    pthread_mutex_lock(&g_arena.lock);

    void    *new_ptr = NULL;
    t_chunk *chunk   = NULL;
    size_t  old_size = 0;
    size_t  new_size = ALIGN(size);

    if (ptr != NULL && (uintptr_t)ptr % ALIGNMENT != 0) {

        
        arena_error_exit(R_INV_PTR_ERROR);
        pthread_mutex_unlock(&g_arena.lock);
        return (NULL);
	}
    if (size > SIZE_MAX - (ALIGNMENT - 1)) {
        
        pthread_mutex_unlock(&g_arena.lock);
        return (NULL);
    }
    if (ptr == NULL) {

        new_ptr = malloc_internal(size);
        if (new_ptr) {

            VALGRIND_MALLOCLIKE_BLOCK(new_ptr, new_size, 0, 0);
        }
        pthread_mutex_unlock(&g_arena.lock);
        return (new_ptr);
    }
    if ((chunk = data_to_chunk(ptr)) == NULL) {
		arena_error_exit(R_INV_PTR_ERROR);
        pthread_mutex_unlock(&g_arena.lock);
        return (NULL);
	}
    if (size == 0) {

        int err = free_internal(chunk);
        if (err == F_NO_ERROR) {

            VALGRIND_FREELIKE_BLOCK(ptr, 0);
            pthread_mutex_unlock(&g_arena.lock);
            return (NULL);
        }
        arena_error_exit(err);
        pthread_mutex_unlock(&g_arena.lock);
        return (NULL);
	}

    
    old_size = get_size(chunk);
    new_ptr = realloc_internal(ptr, chunk, old_size, new_size);

    if (new_ptr && new_ptr == ptr) {
        
        VALGRIND_RESIZEINPLACE_BLOCK(ptr, old_size, new_size, 0);
    }
    else if (ptr && old_size) {

        VALGRIND_FREELIKE_BLOCK(ptr, 0);
    }

    pthread_mutex_unlock(&g_arena.lock);
    return (new_ptr);
}

PUBLIC void    show_alloc_mem(void) {

    pthread_mutex_lock(&g_arena.lock);

    show_alloc_mem_internal();

    pthread_mutex_unlock(&g_arena.lock);
}


PUBLIC void    show_alloc_mem_ex() {

    pthread_mutex_lock(&g_arena.lock);

    show_alloc_mem_ex_internal();

    pthread_mutex_unlock(&g_arena.lock);
}

PUBLIC int     mallopt(int param, int value) {

    pthread_mutex_lock(&g_arena.lock);

    int ret = mallopt_internal(param, value);
    
    pthread_mutex_unlock(&g_arena.lock);
    
    return (ret);
}