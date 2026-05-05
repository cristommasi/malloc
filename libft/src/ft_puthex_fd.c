/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthex_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <ctommasi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 12:54:22 by ctommasi          #+#    #+#             */
/*   Updated: 2026/05/05 13:19:11 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

void	ft_puthex_fd(unsigned long n, int fd, int mode)
{
	char				buf[sizeof(unsigned long) * 2];
	static const char	*hex;
	static const char	*HEX;
	int					i;
	char				c;

	HEX = "0123456789ABCDEF";
	hex = "0123456789abcdef";
	i = sizeof(buf);
	if (!(mode == HEX_LOWER_CASE || mode == HEX_UPPER_CASE))
		return ;
	if (n == 0)
	{
		write(fd, "00", 2);
		return ;
	}
	while (n > 0)
	{
		if (mode == HEX_LOWER_CASE)
			c = hex[n % 16];
		else if (mode == HEX_UPPER_CASE)
			c = HEX[n % 16];
		buf[--i] = c;
		n /= 16;
	}
	write(fd, &buf[i], sizeof(buf) - i);
}