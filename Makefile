ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME        = libft_malloc_$(HOSTTYPE).so
SYMLINK     = libft_malloc.so

CC			= cc -std=c17
CFLAGS		= -Wall -Wextra -fPIC -fvisibility=hidden
LDFLAGS		= -shared -pthread
RM			= rm -f

SRCS		=	src/init.c \
				src/api/malloc.c \
				src/api/free.c \
				src/api/realloc.c \
				src/api/show_alloc_mem.c \
				src/api/show_alloc_mem_ex.c \
				src/api/mallopt.c \
				src/core/malloc_int.c \
				src/core/free_int.c \
				src/core/realloc_int.c \
				src/core/show_alloc_mem_int.c \
				src/core/show_alloc_mem_ex_int.c \
				src/core/mallopt_int.c \
				src/shared/arena.c \
				src/shared/heap.c \
				src/shared/chunk.c \
				src/shared/chunk_utils.c \
				src/shared/ops_utils.c \
				src/shared/show_utils.c

BINDIR		= bin
BIN		= $(SRCS:%.c=$(BINDIR)/%.o)


all: $(NAME) $(SYMLINK)

$(NAME): $(BIN)
	$(CC) $(LDFLAGS) $(BIN) -o $(NAME)


$(SYMLINK): $(NAME)
	ln -sf $(NAME) $(SYMLINK)

$(BINDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BINDIR)

fclean: clean
	$(RM) $(NAME) $(SYMLINK)

re: fclean all

.PHONY: all clean fclean re