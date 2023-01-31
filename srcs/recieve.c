#include "ft_ping.h"

int getIPHeaderLengthInBytes(struct iphdr *ipptr) {
	/* IP header encode the number of 32 bits packet there is in ip_hl field */
	/* as 32 bits is 4 bytes the ip_hl * (32 / 8) can be reduced to  */
	/* ip_hl * 4 */
	return ipptr->ihl * 4;
}

void printMessageStatistics() {
	struct iphdr iphdr;
	struct icmphdr icmphdr;

	iphdr = *(struct iphdr*)g_ping->ipptr;

	icmphdr = *(struct icmphdr*)g_ping->icmpptr;

	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.1lf ms\n",
			g_ping->msg_len, g_ping->ip_str, icmphdr.un.echo.sequence, iphdr.ttl, g_ping->time.diff_ms);
}

void setMsgPointers() {
	int bytesOffset;
	struct iphdr iphdr;

	g_ping->ipptr = (struct ip*)(&g_ping->databuf);

	// need to do this otherwise undefined runtime behaviour is triggered
	// as the pointer to iphdr is not aligned, because it comes from packet data
	// thus does not have bytes padding
	iphdr = *(struct iphdr*)g_ping->ipptr;
	bytesOffset = getIPHeaderLengthInBytes(&iphdr);

	g_ping->msg_len -= bytesOffset;

	g_ping->icmpptr = (struct icmp*)(&g_ping->databuf[bytesOffset]);
}

void setupReception() {
	struct msghdr *msg;

	msg = &g_ping->msg;
	ft_memset(msg, 0, sizeof(*msg));
	msg->msg_name = &g_ping->sin;
	msg->msg_namelen = sizeof(g_ping->sin);

    msg->msg_iov = g_ping->iov;
    msg->msg_iovlen = 1;

    msg->msg_iov[0].iov_base = g_ping->databuf;
    msg->msg_iov[0].iov_len = sizeof(g_ping->databuf);


    msg->msg_control = &g_ping->control;
    msg->msg_controllen = sizeof(g_ping->control);

    msg->msg_flags = 0;
	// g_ping->rec_flags = 0;
	g_ping->rec_flags = MSG_DONTWAIT;
}

static int isValidMessage(int len) {
	//validate msg len and content
	//return if true error
	//use function validateMessage to rework
	if (len != PACKET_LEN) {
		return FALSE;
	}
	return TRUE;
}

void validateMessage(struct iphdr *ipptr, struct icmphdr *icmptr) {
	uint32_t addr;
	uint16_t id;
	uint16_t sequence;

	addr = ((struct sockaddr_in*)&g_ping->dest_addr)->sin_addr.s_addr;
	id = g_ping->icmp.un.echo.id;
	sequence = g_ping->icmp.un.echo.sequence;
	if (ipptr->saddr != addr) {
		printError("ERROR: %s | Error validating msg\n", "the recieved message comes from an unknown source");
		exit(1);
	}
	if (icmptr->un.echo.id != id) {
		printError("ERROR: %s | Error validating msg\n", "the recieved message has an unexpected identifier");
		exit(1);
	}
	if (icmptr->un.echo.sequence != sequence) {
		printError("ERROR: %s | Error validating msg\n", "the recieved message has an unexpected sequence number");
		exit(1);
	}
}

void recieveMessage( ) {
	int res = 0;

	res = recvmsg(g_ping->socket.sockfd, &g_ping->msg, g_ping->rec_flags);
	if (res == -1 && errno != EWOULDBLOCK) {
		printError("ERROR: %s | Error reading msg\n", strerror(errno));
		cleanPing();
		exit(FAILURE);
	}
	if ((res == -1 && errno == EWOULDBLOCK) || isValidMessage(res) == FALSE) {
		//No message found or not the message we expect to find: (src / dest / id)
		return;
	}
	setTimeoutAlarm();

	// refactor this func at the same time as filling icmp packet with TS
	getRoudTripTime();

	setRecieved();
	updateStatistics();
	g_ping->msg_len = res;
	setMsgPointers();
	printMessageStatistics();
	return ;
}
