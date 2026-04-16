#include "../include/malloc.h"



#define BBLACK    "\033[30;1m"
#define BRED      "\033[31;1m"
#define BGREEN    "\033[32;1m"
#define GREEN    "\033[32m"
#define BYELLOW   "\033[33;1m"
#define YELLOW   "\033[33m"
#define BBLUE     "\033[34;1m"
#define BLUE     "\033[34m"
#define BMAGENTA  "\033[35;1m"
#define MAGENTA  "\033[35m"
#define BCYAN     "\033[36;1m"
#define BWHITE    "\033[37;1m"
#define BRESET    "\033[0m"


void printALL(void) {


    t_heap *HEAPS[3] = {g_arena.TINY, g_arena.SMALL, g_arena.LARGE};

    for (int i = 0; i < 3; i++) {

        t_heap *cur_heap = HEAPS[i];
        char heap_name = 'A';

        while (cur_heap != NULL) {
            printf(BMAGENTA "\n_____________________________________________________\n" BRESET);
            printf(MAGENTA "%c-HEAP                  : (%p)\n", heap_name, cur_heap);
            printf("total_size              : %ld\n"BRESET, cur_heap->total_size);


            printf("-----------------------------------------------\n");
            t_chunk *cur_block = heap_to_chunk(cur_heap);
            void *heap_end = (void *)((char *)cur_heap + sizeof(t_heap) + cur_heap->total_size);
            char block_name = 'a';
            while ((void *)cur_block < heap_end) {

                if (!(cur_block->next == NULL && cur_block->prev == NULL)) {
                    cur_block = (void *)((char *)cur_block + sizeof(t_chunk) + cur_block->size);    
                    continue;
                }
                printf(YELLOW "USED-BLOCK              : %c (%p)\n", block_name, cur_block);
                printf("data_size               : %ld\n", cur_block->size);
                printf("d_s + sizeof(t_chunk)   : %ld\n", cur_block->size + sizeof(t_chunk));
                char *data = chunk_to_data(cur_block);
                printf("data                    : \"");
                size_t i = 0;
                while (i < cur_block->size) {
                	char c = data[i];
                	if (c == '\0')
                		break ;
                	printf("%c", c);
                	i++;
                }
                printf("\"\n" BRESET);



                printf("-----------------------------------------------\n");
                
                block_name += 1;
                cur_block = (void *)((char *)cur_block + sizeof(t_chunk) + cur_block->size);       
            }
            heap_name += 1;
            cur_heap = cur_heap->next;
            printf(BMAGENTA "_____________________________________________________\n" BRESET);
        }


        }

}
