/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:51:21 by ctommasi          #+#    #+#             */
/*   Updated: 2024/10/29 16:51:48 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

t_list	*ft_lstlast(t_list **stack_x)
{
	t_list	*last;

	if (!stack_x || !*stack_x)
		return (NULL);
	last = (*stack_x);
	while (last->next)
		last = last->next;
	return (last);
}
