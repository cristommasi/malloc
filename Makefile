ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME_REAL   = libft_malloc_$(HOSTTYPE).so
NAME		= malloc
SYMLINK     = libft_malloc.so

CC			= cc -std=c17
CFLAGS		= -Wall -Wextra  -fPIC -I./libft -g -O0
LDFLAGS		= -shared
RM			= rm -f

LIBFT_DIR	= ./libft
LIBFT		= $(LIBFT_DIR)/libft.a

SRC			= main.c \
src/init.c \
src/malloc.c \
src/free.c \
src/realloc.c \
src/show_alloc_mem.c \
src/show_alloc_mem_ex.c \
src/mallopt.c \
src/shared/arena.c \
src/shared/heap.c \
src/shared/chunk.c \
src/shared/chunk_info.c \
src/shared/ops_info.c

OBJDIR      = objs			
OBJS		= $(SRC:.c=.o)



#----------------------------------------------------------------------
#			   MALLOC
#----------------------------------------------------------------------
all: $(LIBFT) $(NAME) #$(SYMLINK)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS)  $(OBJS) -L$(LIBFT_DIR) -lft -o $(NAME)
	
#$(LDFLAGS)

# $(SYMLINK): $(NAME)
#     ln -sf $(NAME) $(SYMLINK)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME) 


re: fclean all
#----------------------------------------------------------------------
#				LIBFT
#----------------------------------------------------------------------
libft: $(LIBFT)

$(LIBFT):
	make -C $(LIBFT_DIR)

libft_clean:
	make -C $(LIBFT_DIR) clean

libft_fclean:
	make -C $(LIBFT_DIR) fclean

libft_re:
	make -C $(LIBFT_DIR) re


.PHONY: all clean fclean libft clean_libft fclean_libft