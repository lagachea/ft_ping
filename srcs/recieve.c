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


static int getIPHeaderLengthInBytes(struct ip *ipptr) {
	/* IP header encode the number of 32 bits packet there is in ip_hl field */
	/* as 32 bits is 4 bytes the ip_hl * (32 / 8) can be reduced to  */
	/* ip_hl * 4 */
	return ipptr->ip_hl * 4;
}

void printMessageStatistics(int len) {
	struct icmp *icmptr;
	struct ip *ipptr;
	int bytesOffset;

	ipptr = (struct ip*)g_ping->databuf;

	bytesOffset = getIPHeaderLengthInBytes(ipptr);

	len -= bytesOffset;

	icmptr = (struct icmp*)(g_ping->databuf + bytesOffset);

	/* if verbose add identifier */
	/* if ip */
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.1lf ms\n",
			len, g_ping->ip_str, icmptr->icmp_seq, ipptr->ip_ttl, g_ping->time.diff_ms);
	/* if hostname */
	// printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.1lf ms\n",
	// 		len, g_ping->ip_str, g_ping->ip_str, ntohs(icmptr->icmp_seq), ipptr->ip_ttl, t->diff_ms);
	return;
}

void recieveMessage( ) {
	int res = 0;

	res = recvmsg(g_ping->socket.sockfd, &g_ping->msg, g_ping->rec_flags);
	if (res == -1) {
		printError("ERROR: %s | Error reading msg\n", strerror(errno));
		freePing();
		exit(FAILURE);
	}
	else if (res >= 0) {
		alarm(0);
		getTimeDiff();
		setRecieved();
		updateStatistics();
		printMessageStatistics(res);
		return ;
	}
}
