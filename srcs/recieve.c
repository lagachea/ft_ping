#include "ft_ping.h"

static int getIPHeaderLengthInBytes(struct iphdr *ipptr) {
	/* IP header encode the number of 32 bits packet there is in ip_hl field */
	/* as 32 bits is 4 bytes the ip_hl * (32 / 8) can be reduced to  */
	/* ip_hl * 4 */
	return ipptr->ihl * 4;
}

static void validateMessage(struct iphdr *ipptr, struct icmphdr *icmptr) {
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

void printMessageStatistics(int len) {
	struct icmphdr icmptr;
	struct iphdr iphdr;
	int bytesOffset;

	iphdr = *(struct iphdr*)(&g_ping->databuf);

	bytesOffset = getIPHeaderLengthInBytes(&iphdr);

	len -= bytesOffset;

	icmptr = *(struct icmphdr*)(&g_ping->databuf[bytesOffset]);

	validateMessage(&iphdr, &icmptr);

	/* if verbose add identifier */
	if ((g_ping->options & VERBOSE_OPTION) != 0) {
		printf("%d bytes from %s: icmp_seq=%d ident=%d ttl=%d time=%.1lf ms\n",
				len, g_ping->ip_str, icmptr.un.echo.sequence, icmptr.un.echo.id, iphdr.ttl, g_ping->time.diff_ms);
	}
	else {
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.1lf ms\n",
				len, g_ping->ip_str, icmptr.un.echo.sequence, iphdr.ttl, g_ping->time.diff_ms);
	}
	return;
}

void recieveMessage( ) {
	int res = 0;

	g_ping->step.count = RECIEVE;
	res = recvmsg(g_ping->socket.sockfd, &g_ping->msg, g_ping->rec_flags);
	// if res -1 and errno is EAGAIN || EWOULDBLOCK
	// no message was found
	if (res == -1) {
		printError("ERROR: %s | Error reading msg\n", strerror(errno));
		freePing();
		exit(FAILURE);
	}
	else if (res >= 0) {
		//validate msg
		//return if true error
		resetAlarm();
		getRoudTripTime();
		setRecieved();
		updateStatistics();
		printMessageStatistics(res);
		g_ping->step.count = WAIT;
		return ;
	}
}
