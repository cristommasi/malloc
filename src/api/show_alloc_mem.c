
#include "../malloc_internal.h"

PUBLIC void    show_alloc_mem(void) {

    pthread_mutex_lock(&g_arena.lock);

    if (!g_arena.tiny && !g_arena.small && !g_arena.large) {
        
        pthread_mutex_unlock(&g_arena.lock);
        return;
    }
    show_alloc_mem_internal();

    pthread_mutex_unlock(&g_arena.lock);
}
