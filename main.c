#include "./include/malloc.h"


#define CAPACITY 640000
char heap[CAPACITY] = {0};
size_t heap_size = 0;

void    collect() {
    return;
}

void    free(void *ptr) {
    (void)ptr;
    return ;
}


void    *malloc(size_t size) {

    if (heap_size + size < CAPACITY)
        return NULL;
    void *result = heap + heap_size;
    heap_size += size;
    return NULL;
}


int main(void) {

    
    char *root = malloc(26);

    for (int i = 0; i < 26; ++i) {
        root[i] = i + 'A';
    }
    
    return 0;
}
