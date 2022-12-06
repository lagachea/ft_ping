#include "ft_ping.h"
#include "libft.h"
#include <signal.h>

t_ftping *g_ping;

void setup() {
	int res;
	g_ping->service = NULL;
	g_ping->service = NULL;

	g_ping->uid = getuid();
	if (g_ping->uid != 0)
	{
		res = setuid(0);
		if (res == -1)
		{
			printf("\nERROR:%s\n", strerror(errno));
			exit(1);
		}
	}
	g_ping->pid = getpid();
	// printf("PID=%10d | UID=%10d\n", g_ping->pid, g_ping->uid);
	//
	// printf("node => %s\n", g_ping->node);
	// printf("service => %s\n", g_ping->service);
	// printf("=====\n");


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
printf("IN\n");

	int res;
	t_ftping pingdata;
	char node[40] = "google.com";
	char *host;
	// "9.9.9.9;www.42.fr;192.168.1.1;ms-17;google.com";

	// Parse host from arg
	g_ping = &pingdata;
	host = &(node[0]);
	ft_memset(&pingdata, 0, sizeof(pingdata));
	setup();
	g_ping->node = host;

	getAInfo();

	fillIcmp();

	getSimpleSocket();
	setupRecv();

	setHandlers();
	printf("sending icmp header to %s\n", g_ping->dest_addr.sa_data);
	res = sendto(g_ping->socket.sockfd, &g_ping->icmp, ICMP_ADDR_LEN, 0, &g_ping->dest_addr, g_ping->addrlen);
	if (res == -1) {
		printf("Error:{%s} sending packet to %s\n", strerror(errno), g_ping->dest_addr.sa_data);
	}
	else {
		recieveMsg();
	}


printf("OUT\n");
	freePing();
	return 0;
}
