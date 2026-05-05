/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_put_ul.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <ctommasi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 12:55:14 by ctommasi          #+#    #+#             */
/*   Updated: 2026/05/05 12:55:16 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/libft.h"

void	ft_putul_fd(unsigned long n, int fd)
{
	char	buf[20];
	int		i;

	i = 19;
	if (n == 0)
	{
		write(fd, "0", 1);
		return ;
	}
	while (n > 0)
	{
		buf[i] = (n % 10) + '0';
		n /= 10;
		i--;
	}
	write(fd, &buf[i + 1], 19 - i);
}