#include "ft_ping.h"

static int isIPv4Address(union networkAddress *destValue, char *dest) {
	int count = ft_strcountchr(dest, '.');
	int iter = -1;
	int byte;

	// need 4 .
	if (count != 3) {
		return FALSE;
	}
	// need 4 bytes with value <= 255
	while (++iter < 4) {
		//REFACTO TO PREVENT ATOI ERROR
		byte = ft_atoi(dest);
		if (byte > 255 || byte < 0) {
			return FALSE;
		}
		else {
			destValue->bytes[iter] = (unsigned char)byte;
		}
		dest = ft_strchr(dest, '.') + 1;
	}
	
	return TRUE;
}
static void getDestination(char *dest) {
	union networkAddress destination;
	struct sockaddr_in *sosckaddr_in;

	destination.integer = 0;
	/*
	 * is hostname or ip?
	 * if looks like a.b.c.d and 0 <= a & b & c &d <= 255 => IP
	 * else hostname let getAddInfo validate hostname
	*/
	if (isIPv4Address(&destination, dest) == TRUE) {
		g_ping->state = IP;
		g_ping->canonname = dest;
		g_ping->ip_str = dest;
		if (destination.integer == 0) {
			// address is 0.0.0.0 we should redirect to 127.0.0.1
		}
		sosckaddr_in = (struct sockaddr_in*)(&g_ping->dest_addr);
		sosckaddr_in->sin_family = AF_INET;
		sosckaddr_in->sin_addr.s_addr = destination.integer;
	}
	else {
		g_ping->state = HOSTNAME;
		g_ping->node = dest;
	}
}

static void getOption(char *opt) {
	char opt_value = *(opt + 1);
	char invalid[] = "invalid option  ";

	switch (opt_value) {
		// add a warning when option is doubled
		case 'h':
			printUsage();
			exit(0);
			break;
		case 'v':
			g_ping->options |= VERBOSE_OPTION;
			break;
		default:
			invalid[15] = opt_value;
			printError("ERROR: %s | option not recognized\n", &invalid[0]);
			printUsage();
			exit(1);
	}
}

void parseArguments(int ac, char **av) {
	char *current_arg = NULL;
	int iter = 0;

	while (++iter < ac)
	{
		current_arg = av[iter];
		if (current_arg[0] == '-' && current_arg[1] != '\0') {
			getOption(current_arg);
		}
		else {
			getDestination(current_arg);
		}
	}
}

/* state is set by parsedDestination */
int parsedDestination() {
	if (g_ping->state == 0) {
		return FAILURE;
	}
	return SUCCESS;
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
