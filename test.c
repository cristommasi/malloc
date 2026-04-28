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

		//printf("%s%c" RESET,color,((value >> i) & 1) ? '1' : '0');

		if (i % 8 == 0 && i != 0)
			//printf("_");

		i--;
	}
	//printf(" %s\n", str);
}
#include <stdlib.h>

int main(void)
{

    
printf("%d\n", 0xA004A - 0xA0020);

	
	//printf("%p\n", s2);

	//printf("\n");
	


    


    return (0);
}