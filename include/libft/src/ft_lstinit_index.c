/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstinit_index.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:49:38 by ctommasi          #+#    #+#             */
/*   Updated: 2024/10/29 16:49:41 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

void	ft_lstinit_index(t_list **stack_x)
{
	t_list	*current;
	t_list	*stack_dupe;
	int		count;

	current = *stack_x;
	while (current != NULL)
	{
		count = 0;
		stack_dupe = *stack_x;
		while (stack_dupe != NULL)
		{
			if (current->value > stack_dupe->value)
				count++;
			stack_dupe = stack_dupe->next;
		}
		current->index = count;
		current = current->next;
	}
}
