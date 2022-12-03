#include "ft_ping.h"

void getAInfo() {
	int res;

	res = getaddrinfo(g_ping->node, g_ping->service, &(g_ping->hints), &(g_ping->results));
	if (res < 0) {
		printf("%s\n", gai_strerror(res));
		exit(FAILURE);
	}
	g_ping->dest_addr = *g_ping->results->ai_addr;
	g_ping->addrlen = g_ping->results->ai_addrlen;
	// set dest addr and addrlen
}
