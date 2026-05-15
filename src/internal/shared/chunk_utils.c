#include "../../../include/malloc_internal.h"

size_t		ALIGN(size_t size) {

    return ( ((size) + ALIGNMENT - 1) & ~(ALIGNMENT - 1) );
}

bool	has_flags(t_chunk *chunk, size_t flag) {

	if (!chunk)
		return (false);
	return (chunk->size & L_FLAG_MASK & flag) != 0;
}

void	set_flags(t_chunk *chunk, size_t flag) {

	if (!chunk)
		return ;
	chunk->size |= (flag & L_FLAG_MASK);
}

void	unset_flags(t_chunk *chunk, size_t flag) {

	if (!chunk)
		return ;
	chunk->size &= ~(flag & L_FLAG_MASK);
}

size_t	get_prevsize(t_chunk *chunk) {

	if (!chunk)
		return (0);
	return (chunk->prev_size & L_SIZE_MASK);
}

void	set_prevsize(t_chunk *chunk, size_t size) {

	if (!chunk)
		return ;
	size_t flags = chunk->prev_size & L_FLAG_MASK;
	chunk->prev_size = (size & L_SIZE_MASK) | flags;
}

size_t	get_size(t_chunk *chunk) {

	if (!chunk)
		return (0);
	return (chunk->size & L_SIZE_MASK);
}

void	set_size(t_chunk *chunk, size_t size) {

	if (!chunk)
		return ;
	size_t flags = chunk->size & L_FLAG_MASK;
	chunk->size = (size & L_SIZE_MASK) | flags;
}

bool	already_freed(t_chunk *chunk) {

	return (!has_flags(chunk, IN_USE) && !has_flags(chunk, IS_CIS));
}

bool	is_invalid_memory(t_chunk *chunk) {
	
	return (!has_flags(chunk, IN_USE) && has_flags(chunk, IS_CIS));
}

void	*move_data(void *dest, const void *src, size_t n) {
	
	unsigned char		*dest_temp;
	const unsigned char	*src_temp;
	size_t				i;

	if (!dest && !src)
		return (NULL);
	dest_temp = (unsigned char *)dest;
	src_temp = (const unsigned char *)src;
	if (dest_temp > src_temp)
	{
		i = n;
		while (i > 0)
		{
			i--;
			dest_temp[i] = src_temp[i];
		}
		return (dest);
	}
	i = 0;
	while (i < n)
	{
		dest_temp[i] = src_temp[i];
		i++;
	}
	return (dest);
}




