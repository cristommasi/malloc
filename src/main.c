#include "../include/malloc.h"


void    *ft_malloc(size_t size) {

    if (heap_start == NULL) {
        
        heap_start = heap_init(size);
        if (heap_start == NULL) {
            return (NULL);
        }
        t_block *block = heap_to_block(heap_start);
        void    *data = block_to_data(block);
        return (data);
    }

    
    return (NULL);
}

int main(void) {

    char *str = (char *)ft_malloc(5);
    int i = 0;
    for (; i < 26; i++) {

        str[i] = i + 'A';
    }
    str[i] = '\0';
    printf("%s\n", str);
    return 0;
}
