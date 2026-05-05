#include <unistd.h>

void	*ft_memset(void *s, int c, size_t n)
{
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

#define HEX_LOWER_CASE 0
#define HEX_UPPER_CASE 1


int main(void) {

    ft_putaddrhex_fd((unsigned long)0, 1, 0);
    return 0;
}
