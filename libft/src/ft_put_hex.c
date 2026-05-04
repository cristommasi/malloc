#include "../includes/libft.h"

void	ft_put_hex(unsigned long n, int mode)
{
	char	buf[sizeof(unsigned long) * 2];
	char	*hex;
	char	*HEX;
	int		i;
	char	c;

	HEX = "0123456789ABCDEF";
	hex = "0123456789abcdef";
	i = sizeof(buf);

	if (n == 0)
	{
		write(1, "00", 2);
		return ;
	}

	while (n > 0)
	{
		c = (mode == 1) ? HEX[n % 16] : hex[n % 16];
		buf[--i] = c;
		n /= 16;
	}

	write(1, &buf[i], sizeof(buf) - i);
}