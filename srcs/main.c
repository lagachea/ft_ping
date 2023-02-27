#include "ft_ping.h"

t_ftping *g_ping;

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
