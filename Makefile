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

LIBFT_DIR	= ./include/libft
LIBFT		= $(LIBFT_DIR)/libft.a

SRC			= src/main.c src/malloc.c src/arena.c src/heap.c src/chunk.c src/utils.c 
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
#$(SYMLINK)

re: fclean all
#----------------------------------------------------------------------
#				LIBFT
#----------------------------------------------------------------------
libft: $(LIBFT)

$(LIBFT):
	make -C $(LIBFT_DIR)

clean_libft:
	make -C $(LIBFT_DIR) clean

fclean_libft:
	make -C $(LIBFT_DIR) fclean


.PHONY: all clean fclean libft clean_libft fclean_libft