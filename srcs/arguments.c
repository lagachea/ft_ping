#include "ft_ping.h"
#include "libft.h"
#include <stdio.h>

static void getDest(char *dest) {
	printf("dest: %s\n", dest);
	/*
	 * is hostname or ip?
	*/
}

static void getOpt(char *opt) {
	printf("opt: %s\n", opt);
	char opt_value = *(opt + 1);
	switch (opt_value) {
		case 'h':
			g_ping->options ^= HELP_OPTION;
			break;
		case 'v':
			g_ping->options ^= VERBOSE_OPTION;
			break;
		default:
			printf("ERROR\n");
			/* ERROR unknown option */
			break;
	}
}

void parseArgs(int ac, char **av) {
	// Parse host from arg
	char *current_arg = NULL;
	int iter = 0;

	while (++iter < ac)
	{
		current_arg = av[iter];
		if (*current_arg == '-') {
			getOpt(current_arg);
		}
		else {
			getDest(current_arg);
		}
	}
	g_ping->node = av[ac - 1];
	exit(0);
}

/*
   -v adds:
	only on iputiles 2022... print info about socket and ai family before sending packets
   ident at each response print

   -h adds:
   only prints usage and quit gracefully

   need a flag to tell if ip or hostname
   if not hostname => no DNS query
*/
