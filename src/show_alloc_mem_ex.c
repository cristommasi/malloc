#include "../include/malloc.h"


void    show_alloc_mem_ex_internal(int show_free_zones) {

    t_heap *HEAP_TYPES[HEAP_TYPE_COUNT] = { g_arena.tiny, g_arena.small, g_arena.large};

    write(1, HEX_DUMP_HEADER_TXT, sizeof(HEX_DUMP_HEADER_TXT));

    for (int i = 0; i < HEAP_TYPE_COUNT; i++) {

        if (!HEAP_TYPES[i])
            continue;
        char *heap_addr = (char *)heap_to_chunk(HEAP_TYPES[i]);
        char *heap_end  = heap_addr + HEAP_TYPES[i]->total_size;

        while (heap_addr < heap_end) {

            t_chunk *cur_chunk = (t_chunk*)heap_addr;
            heap_addr = heap_addr + print_data_in_chunk(cur_chunk, show_free_zones);
        }
    }
   
}

size_t  print_data_in_chunk(t_chunk *cur_chunk, int show_free_zones) {

    size_t  chunk_size  = get_size(cur_chunk);
   
    if (chunk_size == 0)
        return (16 + sizeof(t_chunk));

    if (show_free_zones  == M_SHOW_INUSE && (!has_flags(cur_chunk, IN_USE) || has_flags(cur_chunk, IS_CIS)))
        return (chunk_size + sizeof(t_chunk));
    else if (show_free_zones == M_SHOW_INUSE_FREE && has_flags(cur_chunk, IS_CIS))
        return (chunk_size + sizeof(t_chunk));

    char    *data_addr = (char *)cur_chunk + sizeof(t_chunk);
    char    *data_end  = data_addr + chunk_size;

    while (data_addr < data_end) {

        ft_puthexaddr_fd((uintptr_t)data_addr, STDOUT_FILENO, HEX_LOWER_CASE);
        ft_putstr_fd(": ", STDOUT_FILENO);
        print_data_bytes_hex(data_addr);
        print_data_bytes_ascii(data_addr);
        write(STDOUT_FILENO, "\n", 1);
        data_addr = data_addr + 16;
    }
    return (chunk_size + sizeof(t_chunk));  
}

void    print_data_bytes_hex(char *data) {

    for (int i = 0; i < 16; i++) {

        unsigned char byte = (unsigned char)data[i];
        ft_puthexbyte_fd(byte, STDOUT_FILENO, HEX_LOWER_CASE);
        if ((i + 1) % 2 == 0)
            write(STDOUT_FILENO, " ", 2);
    }
    write(STDOUT_FILENO, " ", 1);
}

void    print_data_bytes_ascii(char *data) {

    for (int i = 0; i < 16; i++) {

        unsigned char byte = (unsigned char)data[i];
        if (ft_isprint(byte))
            write(STDOUT_FILENO, &byte, 1);
        else
            write(STDOUT_FILENO, ".", 1);
    }
}
