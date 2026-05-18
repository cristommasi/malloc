ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME        = libft_malloc_$(HOSTTYPE).so
SYMLINK     = libft_malloc.so

CC			= cc -std=c17
CFLAGS		= -Wall -Wextra -Werror -fPIC -fvisibility=hidden
LDFLAGS		= -shared
RM			= rm -f


SRCS		=	src/init.c \
				src/internal/malloc.c \
				src/internal/free.c \
				src/internal/realloc.c \
				src/internal/show_alloc_mem.c \
				src/internal/show_alloc_mem_ex.c \
				src/internal/mallopt.c \
				src/internal/shared/arena.c \
				src/internal/shared/heap.c \
				src/internal/shared/chunk.c \
				src/internal/shared/chunk_utils.c \
				src/internal/shared/ops_utils.c \
				src/internal/shared/show_utils.c

OBJDIR		= objs
OBJS		= $(SRCS:%.c=$(OBJDIR)/%.o)


all: $(NAME) $(SYMLINK)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(NAME)

#$(LDFLAGS)
$(SYMLINK): $(NAME)
	ln -sf $(NAME) $(SYMLINK)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJDIR)

fclean: clean
	$(RM) $(NAME) $(SYMLINK)

re: fclean all

.PHONY: all clean fclean re