#include "ft_ping.h"

static void getAddressInformation() {
	int res;

	res = getaddrinfo(g_ping->node, g_ping->service, &g_ping->hints, &g_ping->results);
	if (res < 0) {
		printError("ERROR: %s for \"%s\" | Error getting addrinfo\n", gai_strerror(res), g_ping->node);
		exit(FAILURE);
	}
}

static int parseIPv4Address(union networkAddress *destValue, char *dest) {
	int ret = inet_pton(AF_INET, dest, destValue);
	if (ret == 1) {
		return SUCCESS;
	}
	return FAILURE;
}

static int hasDestination() {
	if ((g_ping->state & (IP | HOSTNAME)) != 0) {
		return TRUE;
	}
	return FALSE;
}

static void getDestination(char *dest) {
	struct sockaddr_in *sockaddr_in;

	if (hasDestination() == TRUE) {
		printError("ERROR: destination %s (%s) is already set cannot add destination %s | Usage error\n",
			g_ping->canonname, g_ping->ip_str, dest);
		printUsage();
		exit(1);
	}

	sockaddr_in = (struct sockaddr_in*)(&g_ping->dest_addr);
	if (parseIPv4Address(&g_ping->destination, dest) == SUCCESS) {
		g_ping->state |= IP;
		g_ping->canonname = dest;
		g_ping->ip_str = dest;
		sockaddr_in->sin_family = AF_INET;
		sockaddr_in->sin_addr.s_addr = g_ping->destination.integer;
	}
	else {
		g_ping->state |= HOSTNAME;
		g_ping->node = dest;

		getAddressInformation();

		ft_memcpy(sockaddr_in, g_ping->results->ai_addr, sizeof(struct sockaddr));
		ft_memcpy(g_ping->canonname, g_ping->results->ai_canonname, ft_strlen(g_ping->results->ai_canonname));

		freeaddrinfo(g_ping->results);
		g_ping->results = NULL;

		g_ping->destination.integer = sockaddr_in->sin_addr.s_addr;
		inet_ntop(AF_INET, &sockaddr_in->sin_addr, g_ping->ip_str, INET_ADDRSTRLEN);
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
