#include <stdlib.h>
#include "./include/malloc.h"


int main(void) {


    char* s1 = malloc(512);

    char* s2 = malloc(512);
    char* s3 = malloc(512);

    char* s4 = malloc(512);

    
    free(s2);


    char *s5 = realloc(s3, 600);


    free(s1);
    free(s4);
    free(s5);

    return (0);
}