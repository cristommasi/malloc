/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 11:23:56 by ctommasi          #+#    #+#             */
/*   Updated: 2024/09/20 11:24:00 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

static void	*ft_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (ptr == NULL)
		return (NULL);
	return (ptr);
}

static void	ft_free_all(char **res, size_t i)
{
	while (i > 0)
	{
		free(res[i - 1]);
		i--;
	}
	free(res);
}

static size_t	ft_array_len(char const *s, char c)
{
	size_t	i;
	size_t	len;
	size_t	check;

	check = 0;
	len = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] == c)
		{
			if (check == 1)
			{
				len++;
				check = 0;
			}
		}
		else
			check = 1;
		i++;
	}
	if (check == 1)
		len++;
	return (len);
}

static size_t	ft_word_len(char const *s, char c, size_t start)
{
	size_t	len;

	len = 0;
	while (s[start + len] && s[start + len] != c)
		len++;
	return (len);
}

char	**ft_split(char const *s, char c)
{
	size_t	i;
	size_t	j;
	size_t	k;
	char	**res;

	i = 0;
	j = 0;
	res = (char **)ft_malloc((ft_array_len(s, c) + 1) * sizeof(char *));
	if (res == NULL)
		return (ft_free_all(res, i), NULL);
	while (i < ft_array_len(s, c))
	{
		while (s[j] == c)
			j++;
		res[i] = (char *)ft_malloc((ft_word_len(s, c, j) + 1) * sizeof(char));
		if (!res[i])
			return (ft_free_all(res, i), NULL);
		k = 0;
		while (s[j] && s[j] != c)
			res[i][k++] = s[j++];
		res[i][k] = '\0';
		i++;
	}
	res[i] = NULL;
	return (res);
}
