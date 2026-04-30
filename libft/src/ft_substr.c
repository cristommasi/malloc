/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 17:38:09 by ctommasi          #+#    #+#             */
/*   Updated: 2024/09/19 17:38:11 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

static char	*ft_malloc_one(char *substring)
{
	substring = (char *)malloc(1);
	if (!substring)
		return (NULL);
	substring[0] = '\0';
	return (substring);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	str_len;
	size_t	substr_len;
	char	*substring;

	substring = NULL;
	if (!s)
		return (NULL);
	str_len = ft_strlen(s);
	if (start >= str_len)
		return (ft_malloc_one(substring));
	substr_len = str_len - start;
	if (substr_len > len)
		substr_len = len;
	substring = (char *)malloc(substr_len + 1);
	if (!substring)
		return (NULL);
	i = 0;
	while (i < substr_len && s[start + i])
	{
		substring[i] = s[start + i];
		i++;
	}
	substring[i] = '\0';
	return (substring);
}
