#include <stdint.h>
#include <stdio.h>
typedef enum {NONE, TINY_HEAP, SMALL_HEAP, LARGE_HEAP} t_heap_type;



int main(void) {

    printf("size = %zu\n", sizeof(t_heap_type));
    return 1;
}