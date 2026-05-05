/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthexaddr_fd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <ctommasi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 13:17:45 by ctommasi          #+#    #+#             */
/*   Updated: 2026/05/05 15:28:10 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"

void    ft_puthexaddr_fd(uintptr_t n, int fd, int mode)
{
    char                buf[sizeof(uintptr_t) * 2];
    static const char   *hex;
    static const char   *HEX;
    int                 i;

    if (!(mode == HEX_LOWER_CASE || mode == HEX_UPPER_CASE))
        return ;
    HEX = "0123456789ABCDEF";
    hex = "0123456789abcdef";
    i = sizeof(buf);
    while (n > 0)
    {
        if (mode == HEX_LOWER_CASE)
            buf[--i] = hex[n % 16];
        else if (mode == HEX_UPPER_CASE)
            buf[--i] = HEX[n % 16];
        n /= 16;
    }
    while (i > 0)
        buf[--i] = '0';
    write(fd, "0x", 2);
    write(fd, buf, sizeof(buf));
}