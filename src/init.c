#include "../include/malloc.h"

    pthread_mutex_t	g_lock;
    t_arena         g_arena = {0};

__attribute__((constructor))
static void malloc_ctor(void) {


    pthread_mutex_init(&g_lock, NULL);
    pthread_mutex_lock(&g_lock);

    g_arena.OPS.CHECK           = _M_CHECK_DEFAULT;
    g_arena.OPS.PERTURB         = _M_PERTURB_DEFAULT;
    g_arena.OPS.ARENA_MAX       = _M_ARENA_MAX_DEFAULT;
    g_arena.OPS.MMAP_THRESHOLD  = _M_MMAP_T_DEFAULT;

    char *val = NULL;

	if ((val = getenv("_MALLOC_CHECK_")) != NULL) {
		ft_mallopt(_MALLOC_CHECK_PARAM_, ft_atoi(val));
	}
	if ((val = getenv("_MALLOC_PERTURB_")) != NULL) {
		ft_mallopt(_MALLOC_PERTURB_PARAM_, ft_atoi(val));
	}
	if ((val = getenv("_MALLOC_ARENA_MAX_")) != NULL) {
		ft_mallopt(_MALLOC_ARENA_MAX_PARAM_, ft_atoi(val));
	}
	if ((val = getenv("_MALLOC_MMAP_THRESHOLD_")) != NULL) {
		ft_mallopt(_MALLOC_MMAP_THRESHOLD_PARAM_, ft_atoi(val));
	}
    pthread_mutex_unlock(&g_lock);
}

__attribute__((destructor))
static void malloc_dtor(void) {

    pthread_mutex_destroy(&g_lock);
}


void	*ft_malloc(size_t size) {

	pthread_mutex_lock(&g_lock);

	void *ptr = malloc_internal(size);

	pthread_mutex_unlock(&g_lock);
	
	return (ptr);
}

void	ft_free(void *ptr) {

	pthread_mutex_lock(&g_lock);

	int ret = free_internal(ptr);
    if (ret == -1) {

        write(2, F_MUNMAP_MSG, sizeof(F_MUNMAP_MSG));
    }
	if (ret == F_DOUBLE_FREE_ERROR) {

		write(2, F_DOUBLE_FREE_MSG, sizeof(F_DOUBLE_FREE_MSG));
	}
	else if (ret == F_INV_PTR_ERROR) {

		write(2, F_INV_PTR_MSG, sizeof(F_INV_PTR_MSG));
	}
	
	pthread_mutex_unlock(&g_lock);
}

void	*ft_realloc(void *ptr, size_t size) {

    pthread_mutex_lock(&g_lock);

    void *new_ptr = realloc_internal(ptr, size);

    pthread_mutex_unlock(&g_lock);
    return (new_ptr);
}

void    show_alloc_mem(void) {

    pthread_mutex_lock(&g_lock);

    show_alloc_mem_internal();

    pthread_mutex_unlock(&g_lock);
}

int     ft_mallopt(int param, int value) {

    pthread_mutex_lock(&g_lock);

    int ret = mallopt_internal(param, value);

    pthread_mutex_unlock(&g_lock);
    return (ret);
}