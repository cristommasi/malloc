/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 10:25:25 by ctommasi          #+#    #+#             */
/*   Updated: 2024/09/24 10:25:29 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

static int	ft_int_len(int n)
{
	int	count;

	count = 0;
	if (n != 0)
	{
		if (n < 0)
		{
			n = n *(-1);
			count++;
		}
		while (n != 0)
		{
			n = n / 10;
			count++;
		}
	}
	else
		count = 1;
	return (count);
}

char	*ft_itoa(int n)
{
	int			len;
	char		*res;
	long int	nbr;

	len = ft_int_len(n);
	nbr = n;
	res = malloc((len + 1));
	if (!res)
		return (NULL);
	if (nbr < 0)
	{
		res[0] = '-';
		nbr = -nbr;
	}
	if (nbr == 0)
		res[0] = '0';
	res[len--] = '\0';
	while (nbr)
	{
		res[len] = ((nbr % 10) + '0');
		nbr /= 10;
		len--;
	}
	return (res);
}
