#include "../include/malloc.h"

pthread_mutex_t	g_lock = PTHREAD_MUTEX_INITIALIZER;
t_arena         g_arena = {0};

__attribute__((constructor))
static void malloc_ctor(void) {


    pthread_mutex_init(&g_lock, NULL);

    g_arena.OPS.CHECK           = _M_CHECK_DEFAULT;
    g_arena.OPS.PERTURB         = _M_PERTURB_DEFAULT;
    g_arena.OPS.ARENA_MAX       = _M_ARENA_MAX_DEFAULT;
    g_arena.OPS.MMAP_THRESHOLD  = _M_MMAP_T_DEFAULT;


    char *val = NULL;

	if ((val = getenv("MALLOC_CHECK_")) != NULL) {
		mallopt_internal(_MALLOC_CHECK_PARAM_, ft_atoi(val));
	}
	if ((val = getenv("MALLOC_PERTURB_")) != NULL) {
		mallopt_internal(_MALLOC_PERTURB_PARAM_, ft_atoi(val));
	}
	if ((val = getenv("MALLOC_ARENA_MAX")) != NULL) {
		mallopt_internal(_MALLOC_ARENA_MAX_PARAM_, ft_atoi(val));
	}
	if ((val = getenv("MALLOC_MMAP_THRESHOLD_")) != NULL) {
		mallopt_internal(_MALLOC_MMAP_THRESHOLD_PARAM_, ft_atoi(val));
	}
}

__attribute__((destructor))
static void malloc_dtor(void) {

    pthread_mutex_destroy(&g_lock);
}


void	*ft_malloc(size_t size) {

	pthread_mutex_lock(&g_lock);

	void *ptr = malloc_internal(size);

    if (ptr != NULL) {

        VALGRIND_MALLOCLIKE_BLOCK(ptr, ALIGN(size), 0, 0);
    }

	pthread_mutex_unlock(&g_lock);

	return (ptr);
}

void	ft_free(void *ptr) {

	pthread_mutex_lock(&g_lock);

	free_internal(ptr);

    VALGRIND_FREELIKE_BLOCK(ptr, 0);

	pthread_mutex_unlock(&g_lock);
}

void	*ft_realloc(void *ptr, size_t size) {

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

void    show_alloc_mem(void) {

    pthread_mutex_lock(&g_lock);

    show_alloc_mem_internal();

    pthread_mutex_unlock(&g_lock);
}


void    show_alloc_mem_ex(int show_type) {

    pthread_mutex_lock(&g_lock);

    show_alloc_mem_ex_internal(show_type);

    pthread_mutex_unlock(&g_lock);
}

int     ft_mallopt(int param, int value) {

    pthread_mutex_lock(&g_lock);

    int ret = mallopt_internal(param, value);
    
    pthread_mutex_unlock(&g_lock);
    
    return (ret);
}