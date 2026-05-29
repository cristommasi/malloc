#include "../malloc_internal.h"

int		size_exceeds_rlimit(size_t aligned_size) {

    struct rlimit   rl;

    if (getrlimit(RLIMIT_AS, &rl) == -1)
        return (0);

    if (rl.rlim_cur == RLIM_INFINITY)
        return (aligned_size > USERSPACE_MAX);

    return ((rlim_t)aligned_size > rl.rlim_cur);
}

void	*move_data(void *dest, const void *src, size_t n) {
	
	unsigned char		*dest_temp;
	const unsigned char	*src_temp;
	size_t				i;

	if (!dest && !src)
		return (NULL);
	dest_temp = (unsigned char *)dest;
	src_temp = (const unsigned char *)src;
	if (dest_temp > src_temp)
	{
		i = n;
		while (i > 0)
		{
			i--;
			dest_temp[i] = src_temp[i];
		}
		return (dest);
	}
	i = 0;
	while (i < n)
	{
		dest_temp[i] = src_temp[i];
		i++;
	}
	return (dest);
}

void	*do_perturb(void *s, unsigned int c, size_t n) {
    
	size_t			i;
	unsigned char	*temp;

	temp = (unsigned char *)s;
	i = 0;
	while (i < n)
	{
		temp[i] = (unsigned char)c;
		i++;
	}
	return (s);
}

int     asciitoint(const char *str) {

	long	res;
	int		minus;
	int		i;

	i = 0;
	res = 0;
	minus = 1;
	while (str[i] == 32 || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-')
		minus *= -1;
	if (*str == '-' || *str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		res = res * 10 + *str - '0';
		str++;
	}
	return (res * minus);
}

void    print_hex_byte(unsigned char byte, int mode) {

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
    write(STDOUT_FILENO, &hex_type[byte >> 4], 1);
    write(STDOUT_FILENO, &hex_type[byte & 0x0F], 1);
}

void    print_hex_addr(uintptr_t n, int mode) {

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
    write(STDOUT_FILENO, "0x", 2);
    write(STDOUT_FILENO, buf, sizeof(buf));
}

void	print_long(unsigned long n) {

	char	buf[20];
	int		i;

	i = 19;
	if (n == 0)
	{
		write(STDOUT_FILENO, "0", 1);
		return ;
	}
	while (n > 0)
	{
		buf[i] = (n % 10) + '0';
		n /= 10;
		i--;
	}
	write(STDOUT_FILENO, &buf[i + 1], 19 - i);
}

void	print_string(char *s) {

	size_t	i;

	i = 0;
	while (s[i] != '\0')
	{
		write(STDOUT_FILENO, &s[i], 1);
		i++;
	}
}

size_t  print_data_in_chunk(char *cur_chunk, size_t chunk_size, int offset) {

	char    *data_addr = (char *)cur_chunk + offset;
	char    *data_end  = data_addr + chunk_size;

	while (data_addr < data_end) {

		print_hex_addr((uintptr_t)data_addr, HEX_LOWER_CASE);
		print_string(": ");
		print_data_bytes(data_addr, 16);
		data_addr = data_addr + 16;
	}
	return (0);  
}

void    print_data_bytes(char *data, size_t len) {

    for (int i = 0; i < 16; i++) {

        if (i < (int)len) {

            unsigned char byte = (unsigned char)data[i];
            print_hex_byte(byte, HEX_LOWER_CASE);
        }
		else
            write(STDOUT_FILENO, "  ", 2);
        if ((i + 1) % 2 == 0)
            write(STDOUT_FILENO, " ", 1);
    }
    write(STDOUT_FILENO, " ", 1);
    for (int i = 0; i < 16; i++) {

        if (i < (int)len) {

            unsigned char byte = (unsigned char)data[i];
            if (byte >= 32 && byte <= 126)
                write(STDOUT_FILENO, &byte, 1);
            else
                write(STDOUT_FILENO, ".", 1);
        }
		else
            write(STDOUT_FILENO, " ", 1);
    }
	write(STDOUT_FILENO, "\n", 1);
}

void    print_heap_type(int index, t_heap *cur) {

	if (!cur)
		return ;
	
	if (index == 0)
		print_string("TINY : ");
	else if (index == 1)
		print_string("SMALL : ");
	else if (index == 2)
		print_string("LARGE : ");

	print_hex_addr((uintptr_t)cur, HEX_UPPER_CASE);
	write(STDOUT_FILENO, "\n", 1);
}

void	print_chunk_addr(t_chunk *cur_chunk, size_t chunk_size) {

	char *data_addr         = (char*)cur_chunk;
	char *addr_end          = (char*)cur_chunk + CHUNK_INUSE_SIZE + chunk_size;

	print_hex_addr((uintptr_t)data_addr, HEX_UPPER_CASE);
	print_string(" - ");
	print_hex_addr((uintptr_t)addr_end, HEX_UPPER_CASE);
	print_string(" : ");
	print_long((uintptr_t)chunk_size);
	print_string(" bytes\n");
}