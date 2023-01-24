#include "ft_ping.h"

void printMsghdr() {
	struct msghdr *msg;
	struct sockaddr_in *sosckaddr_in;
	char rslv_node[INET_ADDRSTRLEN];

	msg = &g_ping->msg;
	print_memory(msg, sizeof(*msg));
	sosckaddr_in = (struct sockaddr_in*)(msg->msg_name);

	printf("Family: %s | Port: %hu | Addr: %s\n", 
			getFamilyName(sosckaddr_in->sin_family)
			, sosckaddr_in->sin_port
			, inet_ntop(AF_INET, &sosckaddr_in->sin_addr.s_addr, &rslv_node[0], INET_ADDRSTRLEN)
			);
}


static int getIPHeaderLengthInBytes(struct iphdr *ipptr) {
	/* IP header encode the number of 32 bits packet there is in ip_hl field */
	/* as 32 bits is 4 bytes the ip_hl * (32 / 8) can be reduced to  */
	/* ip_hl * 4 */
	return ipptr->ihl * 4;
}

static int validateMessage(struct iphdr *ipptr, struct icmp *icmptr) {
	// check if ip src of msg is dest of last sent packet
	// check if icmp type is echo reply seq and id matches the last sent packet
	if (ipptr->saddr != g_ping->sin.sin_addr.s_addr
			|| icmptr->icmp_type != ICMP_ECHOREPLY
			|| icmptr->icmp_seq != g_ping->icmp.icmp_seq
			|| icmptr->icmp_id != g_ping->icmp.icmp_id) {
		return FAILURE;
	}
	return SUCCESS;
}

void printMessageStatistics(int len) {
	struct icmp icmptr;
	struct iphdr ipptr;
	int bytesOffset;

	ipptr = *(struct iphdr*)(&g_ping->databuf);

	bytesOffset = getIPHeaderLengthInBytes(&ipptr);

	len -= bytesOffset;

	icmptr = *(struct icmp*)(&g_ping->databuf[bytesOffset]);

	if (validateMessage(&ipptr, &icmptr) == FAILURE) {
		// message was invalid print
	}

	/* if verbose add identifier */
	if ((g_ping->options & VERBOSE_OPTION) != 0) {
		printf("%d bytes from %s: icmp_seq=%d ident=%d ttl=%d time=%.1lf ms\n",
				len, g_ping->ip_str, icmptr.icmp_seq, icmptr.icmp_id, ipptr.ip_ttl, g_ping->time.diff_ms);
	}
	else {
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.1lf ms\n",
				len, g_ping->ip_str, icmptr.icmp_seq, ipptr.ip_ttl, g_ping->time.diff_ms);
	}
	return;
}

void recieveMessage( ) {
	int res = 0;

	g_ping->step.count = RECIEVE;
	res = recvmsg(g_ping->socket.sockfd, &g_ping->msg, g_ping->rec_flags);
	if (res == -1) {
		printError("ERROR: %s | Error reading msg\n", strerror(errno));
		freePing();
		exit(FAILURE);
	}
	else if (res >= 0) {
		alarm(0);
		getRoudTripTime();
		setRecieved();
		updateStatistics();
		printMessageStatistics(res);
		g_ping->step.count = WAIT;
		return ;
	}
}
