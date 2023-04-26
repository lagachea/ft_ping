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
	int valid_msg;

	g_ping = &pingdata;

	setup();

	parseArguments(ac, av);

	setRawSocket();

	printInitialInformation();

	// reverseDNSquery();
	// exit(255);

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
			valid_msg = hasValidMessage();
			if (valid_msg == TRUE) {
				// a msg was found and is the response we expect to parse

				// give us another TIMEOUT time to work
				setTimeoutAlarm();

				setMsgPointer();

				setRoudTripTime();

				setRecieved();

				updateStatistics();

				printMessageStatistics();
			}
			else if (valid_msg == FALSE) {
				//No valid message found or not the message we expect to find: (src / dest / id)
				// Check for error type messages
				// branch to print error message
				// reverse DNS for iphdr.saddr
				printf("ERROR MSG\n");
			}
		}
	}
	
	// Should never go out this way
	// either exit Failure (timeout or other)
	// or exit Success when SIGINT is sent with Ctrl-C
	return -1;
}
