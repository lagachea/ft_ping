#include "ft_ping.h"

static void getAddressInformation() {
	int res;

	res = getaddrinfo(g_ping->node, g_ping->service, &g_ping->hints, &g_ping->results);
	if (res < 0) {
		printError("ERROR: %s for \"%s\" | Error getting addrinfo\n", gai_strerror(res), g_ping->node);
		exit(FAILURE);
	}
}

static int parseByte(char *dest) {
	int state = -1;
	int byte = 0;
	int iter = -1;

	while (++iter < 3 && dest[iter] != '\0' && dest[iter] != '.') {
		state = 0;
		if (iter > 0) {
			byte *= 10;
		}
		if (dest[iter] < '0' || dest[iter] > '9')
			return -1;
		byte += dest[iter] - '0';
	}
	if (state == -1 || byte > 255 || byte < 0) {
		return -1;
	}
	return byte;
}

static int parseIPv4Address(union networkAddress *destValue, char *dest) {
	int count = ft_strcountchr(dest, '.');
	int iter = -1;
	int byte;

	destValue->integer = 0;
	// need 4 .
	if (count != 3) {
		return FAILURE;
	}
	// need 4 bytes with value <= 255
	while (++iter < 4) {
		byte = parseByte(dest);
		if (byte == -1) {
			return FAILURE;
		}
		else {
			destValue->bytes[iter] = (uint8_t)byte;
		}
		if (iter < 3)
			dest = ft_strchr(dest, '.') + 1;
	}
	
	return SUCCESS;
}

static int hasDestination() {
	if ((g_ping->state & (IP | HOSTNAME)) != 0) {
		return TRUE;
	}
	return FALSE;
}

static void getDestination(char *dest) {
	union networkAddress destination;
	struct sockaddr_in *sosckaddr_in;
	struct in_addr addr_in;

	if (hasDestination() == TRUE) {
		printError("ERROR: destination %s (%s) is already set cannot add destination %s | Usage error\n",
			g_ping->canonname, g_ping->ip_str, dest);
		printUsage();
		exit(1);
	}


	// int ret = inet_pton(AF_INET, dest, &destination.bytes);
	// print_memory(&destination, sizeof(union networkAddress));
	// if (ret == 1) {
	/*
	 * is hostname or ip? clever IDEA pton
	 * if looks like a.b.c.d and 0 <= a & b & c &d <= 255 => IP
	 * else hostname let getAddInfo validate hostname
	*/
	if (parseIPv4Address(&destination, dest) == SUCCESS) {
		g_ping->state |= IP;
		g_ping->canonname = dest;
		g_ping->ip_str = dest;
		sosckaddr_in = (struct sockaddr_in*)(&g_ping->dest_addr);
		sosckaddr_in->sin_family = AF_INET;
		sosckaddr_in->sin_addr.s_addr = destination.integer;
	}
	else {
		g_ping->state |= HOSTNAME;
		g_ping->node = dest;

		getAddressInformation();

		ft_memcpy(&g_ping->dest_addr, g_ping->results->ai_addr, sizeof(struct sockaddr));
		g_ping->canonname = &g_ping->hostname[0]; 
		ft_memcpy(g_ping->canonname,  g_ping->results->ai_canonname, ft_strlen(g_ping->results->ai_canonname));

		addr_in = ((struct sockaddr_in*)(&g_ping->dest_addr))->sin_addr;
		destination.integer = addr_in.s_addr;
		inet_ntop(AF_INET, &addr_in, g_ping->ip_str, INET_ADDRSTRLEN);

		freeaddrinfo(g_ping->results);
		g_ping->results = NULL;
	}
	if (destination.integer == 0u 
			|| (destination.integer >= LOCAL_MIN && destination.integer <= LOCAL_MAX)) {
		g_ping->state |= LOCAL;
		printf("LOCAL %x %u\n",destination.integer, destination.integer);
		// if (strcmp("localhost", dest) == 0){
		// 	g_ping->state |= LOCAL;
		// }
		// address is 0.0.0.0 we should redirect to 127.0.0.1
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

	if (hasDestination() == FALSE) {
		printError("ERROR: %s | Usage error\n", "destination address required");
		printUsage();
		exit(1);
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
