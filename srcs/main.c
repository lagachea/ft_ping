#include "ft_ping.h"

t_ftping *g_ping;

void setup() {
	ft_memset(g_ping, 0, sizeof(t_ftping));

	setHandler(SIGINT, &interruptHandler);
	setHandler(SIGQUIT, &sigquitHandler);

	g_ping->service = NULL;
	g_ping->pid = getpid();

	getRawSocket();

	ft_memset(&g_ping->hints, 0, sizeof(struct addrinfo));
	g_ping->hints.ai_flags = AI_CANONNAME;
	g_ping->hints.ai_family = AF_INET;

	g_ping->results = NULL;

	g_ping->ip_str = &g_ping->rslv_node[0];
	g_ping->state = 0;
	g_ping->seq = 0;
	g_ping->counters.min = 0;
	g_ping->counters.max = 0;
	g_ping->counters.sum = 0;
	g_ping->counters.sum2 = 0;
	g_ping->counters.avg = 0;
	g_ping->counters.mdev = 0;
}

int	main(int ac, char **av)
{
	t_ftping pingdata;

	g_ping = &pingdata;

	setup();

	parseArguments(ac, av);

	if (parsedDestination() == FAILURE) {
		printError("ERROR: %s | Usage error\n", "destination address required");
		// print usage error
		exit(1);
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
