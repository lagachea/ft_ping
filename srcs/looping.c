#include "ft_ping.h"

void looping (){

	int res;

	setupOutput();
	res = sendto(g_ping->socket.sockfd, &g_ping->icmp, ICMP_MINLEN, 0, &g_ping->dest_addr, INET_ADDRSTRLEN);
	if (res == -1) {
		printf("Error:%s | sending packet to %s\n", strerror(errno), g_ping->dest_addr.sa_data);
		freePing();
		exit(FAILURE);
	}
	else {
		setAlarmHandler(TIMEOUT, &timeoutHandler);
		// setAlarmHandler(NEXT, &loopHandler);
		setTransmitted();
		recieveMessage();
		// Replace this function to wait 10 sec with gettimeofday and then return (README state)
		setAlarmHandler(NEXT, &loopHandler);
		return;
	}
}
