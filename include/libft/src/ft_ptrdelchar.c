/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ptrdelchar.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaimesan <jaimesan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 12:23:32 by jaimesan          #+#    #+#             */
/*   Updated: 2024/12/13 15:41:32 by jaimesan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

void	ft_ptrdelchar(char *str, const char *chars)
{
	int		i;
	int		j;
	int		len;

	if (!str || !chars)
		return ;
	i = 0;
	j = 0;
	len = ft_strlen(str);
	while (str[i])
	{
		if (i != 0 && i != (len - 1))
			str[j++] = str[i];
		i++;
	}
	str[j] = '\0';
}
