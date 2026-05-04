#include "../include/malloc.h"
void    print_chunk_data_in_hex(char *chunk_data) ;
void    print_chunk_data_in_ascii(char *chunk_data);



void    show_alloc_mem_ex_internal(void) {

    if (!g_arena.tiny && !g_arena.small && !g_arena.large)
        return ;

    t_heap *arena_types[HEAP_TYPE_COUNT] = { g_arena.tiny, g_arena.small, g_arena.large};
    int     h          = 0;

    write(1, "Address\t\tHex bytes\t\t\t\t\tASCII\n", sizeof("Address\t\tHex bytes\t\t\t\t\tASCII\n"));
    while (h < HEAP_TYPE_COUNT) {

        t_heap *cur_heap = arena_types[h];
        while (cur_heap != NULL) {

            char *cur_heap_addr = (char *)cur_heap + sizeof(t_heap);
            char *heap_end   = cur_heap_addr + cur_heap->total_size;

            while (cur_heap_addr < heap_end) {

                char *cur_chunk = cur_heap_addr;
                size_t cur_chunk_size = get_size((t_chunk*)cur_chunk);
                char *chunk_end = cur_chunk + sizeof(t_chunk) + cur_chunk_size;

      
                if (cur_chunk_size != 0 && has_flags((t_chunk*)cur_chunk, IN_USE)) {

                    char *cur_chunk_data = cur_chunk + sizeof(t_chunk);
                    
                    int i = 0;
                    while (cur_chunk_data < chunk_end) {

                        if (i % 16 == 0) {
                            write(1, "0x", 2);
                            ft_put_hex((unsigned long)cur_chunk_data, 1);
                            write(1, ": ", 2);
                        }
                        print_chunk_data_in_hex(cur_chunk_data);
                        print_chunk_data_in_ascii(cur_chunk_data);
                        cur_chunk_data += 16;
                    }
                }
                cur_heap_addr = chunk_end;
            }
            cur_heap = cur_heap->next;
        }
        h++;
    }
}

void    ft_put_hex_byte(unsigned char byte, int mode) {

    static const char hex[16] = "0123456789abcdef";
    static const char HEX[16] = "0123456789ABCDEF";

    if (mode == 0) {

        write(1, &hex[byte >> 4], 1);
        write(1, &hex[byte & 0x0F], 1);
    }
    else {
        write(1, &HEX[byte >> 4], 1);
        write(1, &HEX[byte & 0x0F], 1);
    }
}

void    print_chunk_data_in_hex(char *chunk_data) {

    for (int i = 0; i < 16; i++) {
        ft_put_hex_byte((unsigned char)chunk_data[i], 0);
        write(1, " ", 1);
    }
}

void    print_chunk_data_in_ascii(char *chunk_data) {

    for (int i = 0; i < 16; i++) {

        if (ft_isprint(chunk_data[i]))
            write(1, &chunk_data[i], 1);
        else
            write(1, ".", 1);
    }
    write(1, "\n", 1);
}