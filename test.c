#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define ALIGNMENT (2 * sizeof(size_t))
#define ALIGN(size) (((size) + ALIGNMENT - 1) & ~(ALIGNMENT - 1))


int main(void) {

	printf("%zu\n", ALIGN(0));
	printf("%zu\n", ALIGN(1));

	return 0;
}
