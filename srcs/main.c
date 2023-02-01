#include "ft_ping.h"

t_ftping *g_ping;

static void setup() {
	ft_memset(g_ping, 0, sizeof(t_ftping));

	setHandler(SIGINT, &interruptHandler);
	setHandler(SIGQUIT, &sigquitHandler);
	setHandler(SIGALRM, &timeoutHandler);

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

	while(TRUE) {
		if (g_ping->step.count == READY) {
			setupRoundTrip();
			sendPing();
			setTransmitted();
			setTimeoutAlarm();
			g_ping->step.count = WAIT;
		}
		else if (g_ping->step.count == WAIT) {
			setWaitClock();
			wait_diff = getTimeDiff(&g_ping->time.wait, &g_ping->time.emission);
			if (wait_diff > ONE_SEC) {
				g_ping->step.count = READY;
			}
		}

		if (expectMessage() == TRUE) {
			setupReception();
			recieveMessage();
			if (hasValidMessage() == TRUE) {
				// a msg was found and is the response we expect to parse

				// give us another TIMEOUT time to work
				setTimeoutAlarm();

				// refactor this func at the same time as filling icmp packet with TS
				getRoudTripTime();

				setRecieved();

				updateStatistics();

				printMessageStatistics();
			}
			//No valid message found or not the message we expect to find: (src / dest / id)
		}
	}
	
	// Should never go out this way
	// either exit Failure (timeout or other)
	// or exit Success when SIGINT is sent with Ctrl-C
	return -1;
}
