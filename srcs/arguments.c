#include "ft_ping.h"
#include "libft.h"
#include <stdio.h>

static void getDestination(char *dest) {
	g_ping->node = dest;
	/*
	 * is hostname or ip?
	 * if looks like a.b.c.d and 0 <= a & b & c &d <= 255 => IP
	 * else hostname let getAddInfo validate hostname
	*/
}

static void getOption(char *opt) {
	char opt_value = *(opt + 1);
	switch (opt_value) {
		// add a warning when option is doubled
		case 'h':
			g_ping->options |= HELP_OPTION;
			break;
		case 'v':
			g_ping->options |= VERBOSE_OPTION;
			break;
		default:
			printf("ERROR: option not recognized\n");
			/* ERROR unknown option */
			break;
	}
}

void parseArguments(int ac, char **av) {
	// Parse host from arg
	char *current_arg = NULL;
	int iter = 0;

	while (++iter < ac)
	{
		current_arg = av[iter];
		if (*current_arg == '-') {
			getOption(current_arg);
		}
		else {
			getDestination(current_arg);
		}
	}
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
