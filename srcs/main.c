#include "ft_ping.h"
#include "libft.h"
#include <signal.h>

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
	g_ping->hints.ai_flags = 0;
	g_ping->hints.ai_canonname = NULL;
	g_ping->hints.ai_addr = NULL;
	g_ping->hints.ai_next = NULL;

	g_ping->seq = 1;
}

int	main(void)
{

	t_ftping pingdata;
	char node[40] = "google.com";
	// "9.9.9.9;www.42.fr;192.168.1.1;ms-17;google.com";

	// Parse host from arg
	g_ping = &pingdata;
	setup();
	g_ping->node = &(node[0]);

	looping();
	// loop trap to keep program running until new signal is sent
	while(1) {
	}
<<<<<<< HEAD
=======
	
	// Should never go out this way
	// either exit Failure (timeout or other)
	// or exit Success when SIGINT is sent with Ctrl-C
>>>>>>> 7a438df (set handler set alarm)
	return -1;
}
