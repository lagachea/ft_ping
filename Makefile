NAME = ft_ping
REF = ping

HDRS = includes/ft_ping.h libft/includes/libft.h
SRC = main.c\
	  socket.c\
	  packet.c\
	  signals.c\
	  recieve.c\
	  tools.c\
	  stats.c\
	  arguments.c\
	  error.c\
	  
# debug.c\
# looping.c\


SRCS = $(addprefix srcs/,$(SRCS))

OBJECT = $(addprefix out/,$(SRC:.c=.o))

LIBDIR = libft

LIBA = $(LIBDIR)/libft.a

CFLAGS = -Wall -Wextra -Werror
CFLAGS += -g3 
CFLAGS += -fsanitize=address,leak,undefined -fno-omit-frame-pointer 
# Don't forget to change the lib flags too
# CFLAGS += -fsanitize=memory,undefined

CC = clang

BLUE = "\\033[36m"
RED = "\\033[31m"
WHITE = "\\033[0m"
GREEN = "\\033[32m"
PURPLE = "\\033[35m"

LNECLR = "\\33[2K\\r"

all: $(NAME) $(REF)

$(NAME): $(OBJECT)
	+ make -s -C $(LIBDIR)
	$(CC) -I includes -I libft/includes -lm -o $(NAME) $(OBJECT) $(LIBA) $(CFLAGS)
	sudo setcap cap_net_raw=pe ft_ping
	sed -e '1s/^/[\n/' -e '$$s/,$$/\n]/' out/*.o.json > compile_commands.json
	printf "$(LNECLR)$(GREEN)make done$(WHITE)\n"

out/%.o: srcs/%.c $(HDRS)
	mkdir -p out
	printf "$(LNECLR)$(NAME): $<"
	$(CC) -MJ $@.json -I includes -I libft/includes -o $@ -c $< $(CFLAGS)

debug: all
	clear; sudo gdb ./ft_ping google.com

run: all
	clear; ./ft_ping google.com

test: all
	clear; python3 ./tests/test.py

firewall:
	sudo python3 ./tests/firewall.py
	sudo iptables -L -t nat
	sudo iptables -L

clean:
	$(RM) -rf out
	+ make -s -C $(LIBDIR) clean
	printf "$(PURPLE)clean done$(WHITE)\n"

server: firewall
	clear; sudo python3 ./tests/server.py

fclean:
	$(RM) -rf out $(NAME) server client tmp/ inetutils-2.0
	+ make -s -C $(LIBDIR) fclean
	printf "$(PURPLE)fclean done$(WHITE)\n"

$(REF):
	mkdir -p tmp/
	curl "https://ftp.gnu.org/gnu/inetutils/inetutils-2.0.tar.gz" > tmp/inetutils-2.0.tar.gz
	tar -xf tmp/inetutils-2.0.tar.gz
	cd ./inetutils-2.0; ./configure; make
	cp ./inetutils-2.0/ping/ping .
	sudo chown root:root ping
	sudo chmod u+s ping
	printf "$(LNECLR)$(GREEN)make download done$(WHITE)\n"

re: fclean all

random:
	$(CC) $(CFLAGS) -I includes -I libft/includes -o server srcs/test.c $(LIBA)
	$(CC) $(CFLAGS) -I includes -I libft/includes -o client srcs/testcli.c $(LIBA)

.PHONY: all fclean clean re FORCE test debug firewall
.SILENT: all fclean clean re FORCE $(NAME) $(OBJECT) test debug firewall
