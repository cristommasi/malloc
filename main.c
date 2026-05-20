#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

typedef struct s_chunk {

	size_t			prev_size;
	size_t			size;
	struct s_chunk	*next;
	struct s_chunk	*prev;
}                   t_chunk;

typedef struct s_heap {

	size_t        	p1;
	size_t        	p2;
	size_t        	p3;
	size_t        	alloc_chunks;
	size_t          total_size;
	t_chunk         *free_cis_start;
	struct s_heap   *next;
	struct s_heap   *prev;

}               t_heap;

typedef struct MALLOC_OPS
{
	uint8_t			SHOW_INFO; // free and alloc fill with bits
	uint8_t			PERTURB; // free and alloc fill with bits
	uint8_t			CHECK; // print msg or abort
	uint32_t		ARENA_MAX; //max amount of arenas

} MALLOC_OPS;

typedef struct s_arena {

	pthread_mutex_t		g_lock;
	
	MALLOC_OPS			OPS;
	t_heap  			*tiny;
	t_heap  			*small;
	t_heap  			*large;
	t_chunk 			*fastbin[7];
	t_chunk 			*smallbin[56];
	size_t				heap_count;

}               t_arena;

t_arena         g_arena = {0};

int main(void) {


    printf("sizeof = %zu\n\n", sizeof(t_arena));
    printf("sizeof = %zu\n", sizeof(g_arena.heap_count));
    printf("sizeof = %zu\n", sizeof(g_arena.OPS));
    printf("sizeof = %zu\n", sizeof(g_arena.tiny));
    printf("sizeof = %zu\n", sizeof(g_arena.small));
    printf("sizeof = %zu\n", sizeof(g_arena.large));
	printf("sizeof = %zu\n", sizeof(g_arena.g_lock));
    printf("sizeof = %zu\n", sizeof(g_arena.fastbin));
    printf("sizeof = %zu\n", sizeof(g_arena.smallbin));

    return (0);
}