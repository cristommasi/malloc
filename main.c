#define RESET   "\033[0m"


#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#define IN_USE       0b001
#define IS_CIS       0b010
#define IS_LARGE     0b100
#define TS_FLAG_MASK  0b00000000000000000000000000000111U          // bits 0-2 of small.size
#define TS_SIZE_MASK  0b11111111111111111111111111111000U          // bits 3-31 of small.size
#define L_FLAG_MASK   ((size_t)0x7)
#define L_SIZE_MASK   (~(size_t)0x7)



// THE LAYOUT OF THESE VARS IMPORTANT
typedef struct t_small_tiny
{
	uint32_t	size;
    uint16_t	next_size;
	uint16_t	prev_size;

}	t_small_tiny;


typedef struct s_large {
	
	size_t size;
	
}	t_large;


// Metadata for a single allocated block
typedef struct s_chunk {

	union {
		t_small_tiny	small;
		t_large		    large;
	};
	struct s_chunk	*next;
}                   t_chunk;


void set_size(t_chunk *chunk, size_t size);
size_t get_size(t_chunk *chunk);
void set_prevsize(t_chunk *chunk, size_t size);
size_t get_prevsize(t_chunk *chunk); 
void set_nextsize(t_chunk *chunk, size_t size);
size_t get_nextsize(t_chunk *chunk);
void set_flags(t_chunk *chunk, size_t flag);
void unset_flags(t_chunk *chunk, size_t flag);
bool has_flags(t_chunk *chunk, size_t flag);
bool is_large(t_chunk *chunk);


void	print_binary(size_t value, int width, char *str)
{
	int		i;
	char	*color;

	i = width - 1;
	while (i >= 0)
	{
		/* select color by bit range */
		if (i >= 48)
			color = RED;        /* top 16 bits */
		else if (i >= 32)
			color = GREEN;      /* next 16 bits */
		else
			color = BLUE;       /* lower 32 bits */

		printf("%s%c" RESET,color,((value >> i) & 1) ? '1' : '0');

		if (i % 8 == 0 && i != 0)
			printf("_");

		i--;
	}
	printf(" %s\n", str);
}

int main(void) {

    t_chunk test = {0};  // zero init

    set_flags(&test, IN_USE);
    set_size(&test, 32);
    set_prevsize(&test, 1024);
    set_nextsize(&test, 48);
    print_binary((size_t)test.large.size, 64, "REAL NUM LARGE\n");
    print_binary(get_size(&test), 64, "get_size(&test);");
    print_binary((size_t)get_prevsize(&test) << 48, 64, "get_prevsize(&test);");
    print_binary((size_t)get_nextsize(&test) << 32, 64, "get_nextsize(&test);");
    print_binary((size_t)test.large.size, 64, "PREV - NEXT - SIZE\n");
    set_flags(&test, IS_LARGE);
    print_binary((size_t)test.large.size, 64, "PREV - NEXT - SIZE\n");
    if(has_flags(&test, IS_LARGE))
        printf("IS_LARGE\n");
    unset_flags(&test, IS_LARGE);
    if(has_flags(&test, IS_LARGE))
        printf("IS_LARGE\n");
    else
        printf("NOT_LARGE\n");
    print_binary((size_t)test.large.size, 64, "PREV - NEXT - SIZE\n");
    print_binary(get_size(&test), 64, "get_size(&test)\n");


    // print_binary(get_size(&test), 64, "getsize");


    return 1;
}