
#include "../malloc_internal.h"

PUBLIC int     mallopt(int param, int value) {

    pthread_mutex_lock(&g_arena.lock);

    int ret = mallopt_internal(param, value);
    
    pthread_mutex_unlock(&g_arena.lock);
    
    return (ret);
}