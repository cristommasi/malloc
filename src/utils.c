#include "../include/malloc.h"

void set_size(t_chunk *chunk, size_t size) {

	if (!chunk) return ;
	if (is_large(chunk)) {

		t_chunk copyLARGE = {
			.large.size = chunk->large.size
		};
		chunk->large.size = size;
		if (has_flags(&copyLARGE, IS_CIS))
			set_flags(chunk, IS_CIS);
		if (has_flags(&copyLARGE, IN_USE))
			set_flags(chunk, IS_CIS);
		set_flags(chunk, IS_LARGE);
		return ;
	}
	t_chunk copySMALL = {
		.small.prev_size = chunk->small.prev_size,
		.small.next_size = chunk->small.next_size,
		.small.size = chunk->small.size,
		.small.flags = chunk->small.flags
	};
	chunk->small.size = (uint16_t)size;
	if (has_flags(&copySMALL, IS_CIS))
		set_flags(chunk, IS_CIS);
	if (has_flags(&copySMALL, IN_USE))
		set_flags(chunk, IS_CIS);
}

size_t get_size(t_chunk *chunk) {

	if (!chunk) return 0;
	if (is_large(chunk)) {
		return (chunk->large.size);
	}
	return (size_t)chunk->small.size;
}

void set_prevsize(t_chunk *chunk, size_t size) {

	if (!chunk) return ;
	if (is_large(chunk)) {
		return ;
	}
	chunk->small.prev_size = (uint16_t)size;
}

size_t get_prevsize(t_chunk *chunk) {

	if (!chunk) return 0;
	if (is_large(chunk)) {
		return 0;
	}
	return (size_t)chunk->small.prev_size;
}

void set_nextsize(t_chunk *chunk, size_t size) {

	if (!chunk) return ;
	if (is_large(chunk)) {
		return ;
	}
	chunk->small.next_size = (uint16_t)size;
}

size_t get_nextsize(t_chunk *chunk) {

	if (!chunk) return 0;
	if (is_large(chunk)) {
		return 0;
	}
	return (size_t)chunk->small.next_size;
}

void set_flags(t_chunk *chunk, size_t flag) {

	if (!chunk) return ;
	if (is_large(chunk)) {
		chunk->large.size |= flag;
		return ;
	}
	chunk->small.flags |= (uint16_t)flag;
} 

void unset_flags(t_chunk *chunk, size_t flag) {

	if (!chunk) return ;
	if (is_large(chunk)) {
		chunk->large.size ^= (size_t)flag;
		return ;
	}
	if (has_flags(chunk, flag)) {
		chunk->small.flags &= ~(uint16_t)flag;
		return ;
	}
} 

bool has_flags(t_chunk *chunk, size_t flag) {

	if (!chunk) return false;
	if (is_large(chunk)) {

		return ((chunk->large.size & L_FLAG_MASK) & (size_t)flag);
	}
	return ((chunk->small.flags & TS_FLAG_MASK) & (uint16_t)flag);
}

bool is_large(t_chunk *chunk) {

	if (!chunk) return false;
	return (chunk->large.size & (size_t)IS_LARGE);

}


size_t		get_min(size_t a, size_t b) {

	return (((a <= b) ? a : b));
}

