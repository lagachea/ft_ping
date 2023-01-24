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

	g_ping->ip_str = &g_ping->rslv_node[0];
}

int	main(int ac, char **av)
{
	t_ftping pingdata;
	long int wait_diff;

	g_ping = &pingdata;

	setup();

	parseArguments(ac, av);

	// loop trap to keep program running until new signal is sent
	while(1) {
		if (g_ping->step.count == READY) {
			looping();
		}
		else if (g_ping->step.count == WAIT) {
			updateWaitClock();
			wait_diff = getTimeDiff(&g_ping->time.tvw, &g_ping->time.tvf);
			if (wait_diff > ONE_SEC) {
				g_ping->step.count = READY;
			}
		}
	}
	
	// Should never go out this way
	// either exit Failure (timeout or other)
	// or exit Success when SIGINT is sent with Ctrl-C
	return -1;
}
