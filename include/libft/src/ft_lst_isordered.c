/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lst_isordered.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 12:47:51 by ctommasi          #+#    #+#             */
/*   Updated: 2024/10/30 12:47:53 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

int	ft_lst_isordered(t_list **stack_x)
{
	t_list	*current;
	int		last_value;

	if (!stack_x || !(*stack_x))
		return (1);
	current = *stack_x;
	last_value = current->value;
	while (current != NULL)
	{
		if (last_value > current->value)
			return (0);
		last_value = current->value;
		current = current->next;
	}
	return (1);
}
