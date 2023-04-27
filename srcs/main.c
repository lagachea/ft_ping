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
	int validReply;

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
			validReply = hasValidReply();
			if (validReply == TRUE) {
				// a msg was found and is the response we expect to parse

				// give us another TIMEOUT time to work
				setTimeoutAlarm();

				setMsgPointer();

				setRoudTripTime();

				setRecieved();

				updateStatistics();

				printMessageStatistics();
			}
			else if (validReply == FALSE) {
				//No valid message found or not the message we expect to find: (src / dest / id)

				// Check for error type messages
				// ICMP_DEST_UNREACH
					// reverse DNS for iphdr.saddr
					// printMessageError 
					//Verbose add iphdr dump + icmp info of sent packt
				// ICMP_TIME_EXCEEDED
					// reverse DNS for iphdr.saddr
					// printMessageError 
					//Verbose add iphdr dump + icmp info of sent packt
				//Reuse printMessageStatistics into a printMessageError
				dprintf(STDOUT_FILENO, "%lu bytes ERROR MSG\n", g_ping->msg_ret - sizeof(struct iphdr));
				// ICMP_REDIRECT
					// unknown for now
			}
		}
	}
	
	// Should never go out this way
	// either exit Failure (timeout or other)
	// or exit Success when SIGINT is sent with Ctrl-C
	return -1;
}
