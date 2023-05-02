#include "ft_ping.h"

#ifdef DEBUG
int __lsan_is_turned_off()
{
    return 1;
}
#endif

t_ftping *g_ping;

int	main(int ac, char **av)
{
	t_ftping pingdata;
	long int wait_diff;

	g_ping = &pingdata;

	setup();

	parseArguments(ac, av);

	setRawSocket();

	printInitialInformation();

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

		if (g_ping->counters.transmitted > g_ping->counters.recieved) {
			setupReception();
			recieveMessage();
			if (hasReply() == TRUE) {
				if (isValidReply() == TRUE) {
					handleValidReply();
				}
				else {
					handleInvalidReply();
				}

			}
		}
	}
	
	// Should never go out this way
	// either exit Failure (timeout or other)
	// or exit Success when SIGINT is sent with Ctrl-C
	return -1;
}
