
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




typedef struct t_small_tiny
{
	uint16_t	prev_size;
    uint16_t	next_size;
	uint32_t	size;

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

bool is_large(t_chunk *chunk)
{
	if (!chunk)
		return false;

	return (chunk->large.size & IS_LARGE) != 0;
}
bool has_flags(t_chunk *chunk, size_t flag) {

    if (!chunk)
		return false;

    return (chunk->small.size & TS_FLAG_MASK & (uint32_t)flag) != 0;
}


void set_flags(t_chunk *chunk, size_t flag) {

    if (!chunk)
		return;

    chunk->small.size |= (uint32_t)(flag & TS_FLAG_MASK);
}


void unset_flags(t_chunk *chunk, size_t flag) {

    if (!chunk)
		return;

    chunk->small.size &= ~(uint32_t)(flag & TS_FLAG_MASK);
}


size_t get_size(t_chunk *chunk)
{
    if (!chunk) return 0;

    if (is_large(chunk))
        return chunk->large.size & L_SIZE_MASK;

    return (size_t)(chunk->small.size & TS_SIZE_MASK);
}

void set_size(t_chunk *chunk, size_t size)
{
    if (!chunk) return;

    if (is_large(chunk))
    {
        size_t flags = chunk->large.size & L_FLAG_MASK;
        chunk->large.size = (size & L_SIZE_MASK) | flags;
        return;
    }

    uint32_t flags = chunk->small.size & TS_FLAG_MASK;
    chunk->small.size = (uint32_t)(size & TS_SIZE_MASK) | flags;
}

size_t get_prevsize(t_chunk *chunk) {

    if (!chunk || is_large(chunk))
		return (0);
    return (size_t)chunk->small.prev_size;
}


void set_prevsize(t_chunk *chunk, size_t size) {

    if (!chunk || is_large(chunk))
		return ;
    chunk->small.prev_size = (uint16_t)size;
}



void    print_binary(size_t value, int width)
{
    int i = width - 1;
    while (i >= 0)
    {
        write(1, (value >> i) & 1 ? "1" : "0", 1);
        if (i % 8 == 0 && i != 0)
            write(1, "_", 1);
        i--;
    }
    write(1, "\n", 1);
}

int main(void) {
    t_chunk small = {0};  // zero init
    t_chunk large = {0};  // zero init

    set_flags(&small, IN_USE);
    set_flags(&large, IS_LARGE);
    set_size(&small, 32);
    set_size(&large, 32);

print_binary((size_t)small.small.size, 32);
print_binary(large.large.size, 64);
    return 1;
}