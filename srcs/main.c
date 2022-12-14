#include "ft_ping.h"

t_ftping *g_ping;

void setup() {
	setHandler(SIGINT, &interruptHandler);
	ft_memset(g_ping, 0, sizeof(t_ftping));
	g_ping->service = NULL;


	getSimpleSocket();

	g_ping->pid = getpid();

	g_ping->hints.ai_family = AF_INET;
	g_ping->hints.ai_socktype = 0;
	g_ping->hints.ai_protocol = 0;
	g_ping->hints.ai_flags = AI_CANONNAME;
	g_ping->hints.ai_canonname = NULL;
	g_ping->hints.ai_addr = NULL;
	g_ping->hints.ai_next = NULL;

	g_ping->seq = 1;
	g_ping->ip_str = &g_ping->rslv_node[0];
	g_ping->ip_str2 = &g_ping->rslv_node2[0];
}

int	main(int ac, char **av)
{

	t_ftping pingdata;

	// Parse host from arg
	g_ping = &pingdata;
	setup();
	g_ping->node = av[ac - 1];

	looping();
	// loop trap to keep program running until new signal is sent
	while(1) {
	}
	
	// Should never go out this way
	// either exit Failure (timeout or other)
	// or exit Success when SIGINT is sent with Ctrl-C
	return -1;
}
