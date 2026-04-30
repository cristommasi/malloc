#include "../includes/libft.h"

void	ft_put_ul(unsigned long n)
{
	char	buf[20];
	int		i;

	i = 19;
	if (n == 0)
	{
		write(1, "0", 1);
		return ;
	}
	while (n > 0)
	{
		buf[i] = (n % 10) + '0';
		n /= 10;
		i--;
	}
	write(1, &buf[i + 1], 19 - i);
}