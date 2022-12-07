#include "ft_ping.h"
#include <stdio.h>

void looping (){

	int res;
	setupOutput();
	res = sendto(g_ping->socket.sockfd, &g_ping->icmp, ICMP_PACKET_LEN, 0, &g_ping->dest_addr, g_ping->addrlen);
	if (res == -1) {
		printf("Error:{%s} sending packet to %s\n", strerror(errno), g_ping->dest_addr.sa_data);
		freePing();
		exit(FAILURE);
	}
	else {
		recieveMsg();
		// Set next round alarm
		setAlarmHandler(1, &loopHandler);
		return;
	}
}
