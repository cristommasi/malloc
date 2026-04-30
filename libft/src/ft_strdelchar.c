/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdelchar.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaimesan <jaimesan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 12:26:12 by jaimesan          #+#    #+#             */
/*   Updated: 2024/12/03 12:33:01 by jaimesan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

char	*ft_strdelchar(const char *str, const char *chars)
{
	char	*result;
	int		i;
	int		j;

	i = 0;
	j = 0;
	if (!str || !chars)
		return (NULL);
	result = malloc(ft_strlen(str) + 1);
	if (!result)
		return (NULL);
	while (str[i])
	{
		if (!ft_strchr(chars, str[i]))
			result[j++] = str[i];
		i++;
	}
	result[j] = '\0';
	return (result);
}
