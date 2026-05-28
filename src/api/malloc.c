
#include "../malloc_internal.h"

PUBLIC void	*malloc(size_t size) {

	pthread_mutex_lock(&g_arena.lock);

	void *ptr = malloc_internal(size);

    if (ptr != NULL) {

        VALGRIND_MALLOCLIKE_BLOCK(ptr, ALIGN(size), 0, 0);
    }

	pthread_mutex_unlock(&g_arena.lock);

	return (ptr);
}