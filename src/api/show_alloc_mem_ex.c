
#include "../malloc_internal.h"

PUBLIC void    show_alloc_mem_ex(void) {

    pthread_mutex_lock(&g_arena.lock);

    if (!g_arena.tiny && !g_arena.small && !g_arena.large) {
        
        pthread_mutex_unlock(&g_arena.lock);
        return;
    }
    int show_type   = get_show_info();

    show_alloc_mem_ex_internal(show_type);

    pthread_mutex_unlock(&g_arena.lock);
}