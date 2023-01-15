#include "ft_ping.h"

t_ftping *g_ping;

void setup() {
	ft_memset(g_ping, 0, sizeof(t_ftping));

	setHandler(SIGINT, &interruptHandler);
	setHandler(SIGQUIT, &sigquitHandler);

	g_ping->service = NULL;
	g_ping->pid = getpid();

	getRawSocket();

	g_ping->hints.ai_flags = AI_CANONNAME;
	g_ping->hints.ai_family = AF_INET;
	g_ping->hints.ai_socktype = 0;
	g_ping->hints.ai_protocol = 0;
	g_ping->hints.ai_addr = NULL;
	g_ping->hints.ai_canonname = NULL;
	g_ping->hints.ai_next = NULL;

	g_ping->results = NULL;

	g_ping->ip_str = &g_ping->rslv_node[0];
}

int	main(int ac, char **av)
{
	t_ftping pingdata;

	g_ping = &pingdata;

	setup();

	parseArguments(ac, av);

	if (parsedDestination() == FAILURE) {
		// print usage error
		exit(1);
	}

	if ((g_ping->options & HELP_OPTION) != 0) {
		printUsage();
	}

	looping();
	// loop trap to keep program running until new signal is sent
	while(1) {
	}
	
	// Should never go out this way
	// either exit Failure (timeout or other)
	// or exit Success when SIGINT is sent with Ctrl-C
	return -1;
}
