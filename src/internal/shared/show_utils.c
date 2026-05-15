#include "../../../include/malloc_internal.h"

bool	printable_char(int c) {

	return ((bool)(c >= 32 && c <= 126));
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

		print_hex_addr((uintptr_t)data_addr, 0);
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
            if (printable_char(byte))
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