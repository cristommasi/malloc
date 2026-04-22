#include "../include/malloc.h"

void set_size(t_chunk *chunk, size_t size) {

	if (chunk->vars->info.flags & IS_LARGE) {
		chunk->vars->L_size = size;
		return ;
	}
	chunk->vars->info.size = (uint16_t)size;
}

size_t get_size(t_chunk *chunk) {

	if (chunk->vars->info.flags & IS_LARGE) {
		return chunk->vars->L_size;
	}
	return (size_t)chunk->vars->info.size;
}

void set_prevsize(t_chunk *chunk, size_t size) {

	if (chunk->vars->info.flags & IS_LARGE) {
		return ;
	}
	chunk->vars->info.prev_size = (uint16_t)size;
}

size_t get_prevsize(t_chunk *chunk) {

	if (chunk->vars->info.flags & IS_LARGE) {
		return 0;
	}
	return (size_t)chunk->vars->info.prev_size;
}

void set_nextsize(t_chunk *chunk, size_t size) {

	if (chunk->vars->info.flags & IS_LARGE) {
		return ;
	}
	chunk->vars->info.next_size = (uint16_t)size;
}

size_t get_nextsize(t_chunk *chunk) {

	if (chunk->vars->info.flags & IS_LARGE) {
		return 0;
	}
	return (size_t)chunk->vars->info.next_size;
}

void set_flags(t_chunk *chunk, size_t flag) {

	chunk->vars->info.flags |= (uint16_t)flag;
} 

void unset_flags(t_chunk *chunk, size_t flag) {

	chunk->vars->info.flags ^= (uint16_t)flag;
} 

bool has_flags(t_chunk *chunk, size_t flag) {

	return (chunk->vars->info.flags & flag);
}



size_t		get_min(size_t a, size_t b) {

	return (((a <= b) ? a : b));
}

