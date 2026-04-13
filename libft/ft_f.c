/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_f.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 11:09:03 by ctommasi          #+#    #+#             */
/*   Updated: 2024/09/27 11:09:05 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	*ft_strtoupper(void *str)
{
	size_t	i;
	char	*temp;

	temp = (char *)str;
	i = 0;
	while (temp[i])
	{
		if (temp[i] >= 'a' && temp[i] <= 'z')
			temp[i] = temp[i] - 32;
		i++;
	}
	return (temp);
}

void	*f(void *ptr)
{
	void	*temp;

	if (ptr == NULL)
		return (NULL);
	temp = ft_strtoupper(ptr);
	return (temp);
}
