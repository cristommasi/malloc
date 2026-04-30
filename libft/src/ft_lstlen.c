/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 12:57:20 by ctommasi          #+#    #+#             */
/*   Updated: 2024/10/30 12:57:22 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

size_t	ft_lstlen(t_list **stack_x)
{
	size_t	len;
	t_list	*current;

	len = 0;
	current = *stack_x;
	while (current != NULL)
	{
		current = current->next;
		len++;
	}
	return (len);
}
