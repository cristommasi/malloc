#include "../includes/libft.h"

void	ft_put_hex(unsigned long n)
{
	char	buf[2 + sizeof(unsigned long) * 2];
	char	*hex;
	int		i;

	hex = "0123456789ABCDEF";
	buf[0] = '0';
	buf[1] = 'x';
	if (n == 0)
	{
		write(1, "0x0", 3);
		return ;
	}
	i = 2 + sizeof(unsigned long) * 2 - 1;
	while (n > 0)
	{
		buf[i] = hex[n % 16];
		n /= 16;
		i--;
	}
	write(1, "0x", 2);
	write(1, &buf[i + 1], (2 + sizeof(unsigned long) * 2 - 1) - i);
}