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


ORANGE = \033[38;5;214m
GREEN = \033[38;5;82m
RESET = \033[0m

#----------------------------------------------------------------------
#			   MALLOC
#----------------------------------------------------------------------
all: $(NAME)

$(NAME): $(OBJS) $(LIBFT)
	@echo "$(ORANGE)Linking...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft -o $(NAME) 
	@echo "$(GREEN)Executable built: $(NAME)$(RESET)"

%.o: %.c
	@echo "$(ORANGE)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@


clean:
	@$(RM) $(OBJS)
	@echo "$(GREEN)All object files removed.$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(GREEN)Executable removed.$(RESET)"

re: fclean all
#----------------------------------------------------------------------
#				LIBFT
#----------------------------------------------------------------------
libft: $(LIBFT)

$(LIBFT):
	@echo "$(ORANGE)Building libft...$(RESET)"
	@make $(NO_PRINT) -C $(LIBFT_DIR)
	@echo "$(GREEN)Executable built: libft.a$(RESET)"

clean_libft:
	@make $(NO_PRINT) -C $(LIBFT_DIR) clean
	@echo "$(GREEN)Libft cleaned.$(RESET)"

fclean_libft:
	@make $(NO_PRINT) -C $(LIBFT_DIR) fclean
	@echo "$(GREEN)libft.a executable removed.$(RESET)"


.PHONY: all clean fclean libft clean_libft fclean_libft