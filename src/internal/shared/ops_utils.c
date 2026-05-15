#include "../../../include/malloc_internal.h"

uint8_t     get_show_info(void) {
    
    return (g_arena.OPS.SHOW_INFO);
}

bool    has_perturb(void) {

    return ((bool)g_arena.OPS.PERTURB);
}

int     get_perturb_alloc(void) {

    return ((int)g_arena.OPS.PERTURB);
}

int     get_perturb_free(void) {

    return (~((int)g_arena.OPS.PERTURB));
}


uint8_t     get_check(void) {
    
    return (g_arena.OPS.CHECK);
}

bool    has_check(void) {

    return ((bool)g_arena.OPS.CHECK);
}

bool    has_arena_max(void) {

    return ((bool)g_arena.OPS.ARENA_MAX);
}

uint32_t    get_arena_max(void) {

    return (g_arena.OPS.ARENA_MAX);
}

int     asciitoint(const char *str) {

	long	res;
	int		minus;
	int		i;

	i = 0;
	res = 0;
	minus = 1;
	while (str[i] == 32 || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-')
		minus *= -1;
	if (*str == '-' || *str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		res = res * 10 + *str - '0';
		str++;
	}
	return (res * minus);
}

void	*do_perturb(void *s, int c, size_t n) {
    
	size_t			i;
	unsigned char	*temp;

	temp = (unsigned char *)s;
	i = 0;
	while (i < n)
	{
		temp[i] = (unsigned char)c;
		i++;
	}
	return (s);
}



