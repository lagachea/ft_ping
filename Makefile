NAME = ft_ping

SRC = main.c\
	  debug.c\
	  socket.c\
	  packet.c\
	  signals.c\
	  recieve.c\
	  tools.c\


SRCS = $(addprefix srcs/,$(SRCS))

OBJECT = $(addprefix out/,$(SRC:.c=.o))

LIBDIR = libft

LIBA = $(LIBDIR)/libft.a

CFLAGS = -Wall -Wextra -Werror
CFLAGS += -g -fsanitize=address -fno-omit-frame-pointer

CC = clang
#CC = gcc

BLUE = "\\033[36m"
RED = "\\033[31m"
WHITE = "\\033[0m"
GREEN = "\\033[32m"
PURPLE = "\\033[35m"

LNECLR = "\\33[2K\\r"

all: $(NAME)

$(NAME): $(OBJECT)
	make -s -C $(LIBDIR)
	$(CC) $(CFLAGS) -I includes -I libft/includes -o $(NAME) $(OBJECT) $(LIBA)
	printf "$(LNECLR)$(GREEN)make done$(WHITE)\n"

out/%.o: srcs/%.c
	mkdir -p out
	printf "$(LNECLR)$(NAME): $<"
	$(CC) $(CFLAGS) -I includes -I libft/includes -o $@ -c $<

json:
	bear -- make re -s

test: all
	sudo python3 tests/test.py
	clear; sudo ./ft_ping

clean:
	$(RM) -rf out
	make -s -C $(LIBDIR) clean
	printf "$(PURPLE)clean done$(WHITE)\n"

fclean:
	$(RM) -rf out $(NAME)
	make -s -C $(LIBDIR) fclean
	printf "$(PURPLE)fclean done$(WHITE)\n"

re: fclean all

.PHONY: fclean clean re FORCE json test
.SILENT: fclean clean re FORCE $(NAME) $(OBJECT) json test
