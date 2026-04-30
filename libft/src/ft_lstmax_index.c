/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmax_index.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:46:36 by ctommasi          #+#    #+#             */
/*   Updated: 2024/10/29 16:47:17 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

t_list	*ft_lstmax_index(t_list **stack_x)
{
	t_list	*current;
	t_list	*max_index;

	if (!stack_x || !*stack_x)
		return (NULL);
	current = *stack_x;
	max_index = *stack_x;
	while (current)
	{
		if (current->index > max_index->index)
			max_index = current;
		current = current->next;
	}
	return (max_index);
}
