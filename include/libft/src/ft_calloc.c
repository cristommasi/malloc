/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 13:22:12 by ctommasi          #+#    #+#             */
/*   Updated: 2024/09/19 13:22:14 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	unsigned char	*temp;

	temp = malloc(nmemb * size);
	if (temp == NULL)
		return (NULL);
	ft_bzero(temp, (nmemb * size));
	return (temp);
}
