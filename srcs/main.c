#include "ft_ping.h"
#include "libft.h"

t_ftping *g_ping;

void setup() {
	int res;
	g_ping->service = NULL;
	g_ping->service = NULL;

	g_ping->uid = getuid();
	if (g_ping->uid != 0)
	{
		res = setuid(0) != 0;
		if (res)
			exit(1);
	}
	g_ping->pid = getpid();
	printf("PID=%10d | UID=%10d\n", g_ping->pid, g_ping->uid);

	printf("node => %s\n", g_ping->node);
	printf("service => %s\n", g_ping->service);
	printf("=====\n");


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

	sendto(g_ping->socket.sockfd, &g_ping->icmp, ICMP_ADDR_LEN, 0, &g_ping->dest_addr, g_ping->addrlen);

	close(g_ping->socket.sockfd);
	printf("closed sockfd %d\n",g_ping->socket.sockfd);
	freeaddrinfo(g_ping->results);
	g_ping->results = NULL;


printf("OUT\n");
	return 0;
}
