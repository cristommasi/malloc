#include "../include/malloc.h"

#include <limits.h>

int main(void)
{

	for (int i = 0; i < 129; i++) {

		char *s1 = ft_malloc(16);
	}
	for (int i = 0; i < 129; i++) {

		char *s2 = ft_malloc(129);
	}
	char *s3 = ft_malloc(16);
	char *s4 = ft_malloc(1208);
	show_alloc_mem();


    return (0);
}