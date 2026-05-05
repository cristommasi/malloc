/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthexbyte_fd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <ctommasi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 12:59:42 by ctommasi          #+#    #+#             */
/*   Updated: 2026/05/05 13:30:15 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

void    ft_puthexbyte_fd(unsigned char byte, int fd, int mode)
{

    static const char *hex;
    static const char *HEX;
    const char        *hex_type;

    hex = "0123456789abcdef";
    HEX = "0123456789ABCDEF";
    if (mode == HEX_LOWER_CASE)
        hex_type = hex;
    else if (mode == HEX_UPPER_CASE)
        hex_type = HEX;
    else
        return ;
    write(fd, &hex_type[byte >> 4], 1);
    write(fd, &hex_type[byte & 0x0F], 1);
}