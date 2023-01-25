#include "ft_ping.h"

void pingRoundTrip (){

	int res;

	setupRoundTrip();
	g_ping->step.count = SEND;
	res = sendto(g_ping->socket.sockfd, &g_ping->icmp, ICMP_MINLEN, 0, &g_ping->dest_addr, INET_ADDRSTRLEN);
	if (res == -1) {
		printf("Error:%s | sending packet to %s\n", strerror(errno), g_ping->dest_addr.sa_data);
		freePing();
		exit(FAILURE);
	}
	else {
		setAlarmHandler(TIMEOUT, &timeoutHandler);
		setTransmitted();
		recieveMessage();
		return;
	}
}
