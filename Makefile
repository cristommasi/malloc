ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so

CC = cc
CFLAGS = -Wall -Wextra -Werror -I./libft
RM = rm -f
NO_PRINT = --no-print-directory

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

SRC = main.c
OBJS = $(SRC:.c=.o)



#----------------------------------------------------------------------
#			   MALLOC
#----------------------------------------------------------------------
all: $(NAME)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft -o $(NAME) 

%.o: %.c
	echo "$(ORANGE)Compiling $<...$(RESET)"
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
	make $(NO_PRINT) -C $(LIBFT_DIR)

clean_libft:
	make $(NO_PRINT) -C $(LIBFT_DIR) clean

fclean_libft:
	make $(NO_PRINT) -C $(LIBFT_DIR) fclean


.PHONY: all clean fclean libft clean_libft fclean_libft