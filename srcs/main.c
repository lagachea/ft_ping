#include "ft_ping.h"
#include "libft.h"
#include <signal.h>

t_ftping *g_ping;

void setup() {
	g_ping->service = NULL;


	getSimpleSocket();
	setHandlers();

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

	int res;
	t_ftping pingdata;
	char node[40] = "google.com";
	char *host;
	// "9.9.9.9;www.42.fr;192.168.1.1;ms-17;google.com";

	// Parse host from arg
	g_ping = &pingdata;
	host = &(node[0]);
	ft_memset(&pingdata, 0, sizeof(pingdata));
	g_ping->node = host;
	setup();
	setupInput();

	// int i = -1;
	// while (++i < 3) {
	setupOutput();
	res = sendto(g_ping->socket.sockfd, &g_ping->icmp, ICMP_PACKET_LEN, 0, &g_ping->dest_addr, g_ping->addrlen);
	if (res == -1) {
		printf("Error:{%s} sending packet to %s\n", strerror(errno), g_ping->dest_addr.sa_data);
	}
	else {
		recieveMsg();
	}
	// }


	freePing();
	return 0;
}
