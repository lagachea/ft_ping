#include "ft_ping.h"

int getIPHeaderLengthInBytes(struct iphdr *ipptr) {
	/* IP header encode the number of 32 bits packet there is in ip_hl field */
	/* as 32 bits is 4 bytes the ip_hl * (32 / 8) can be reduced to  */
	/* ip_hl * 4 */
	return ipptr->ihl * 4;
}

void printErrorMsgDump() {
	t_msg_packet msg;
	uint16_t size = g_ping->msg_ret - sizeof(struct iphdr) - sizeof(struct icmphdr) - sizeof(struct iphdr);

	msg = *((t_msg_packet*)(g_ping->databuf + sizeof(struct iphdr) + sizeof(struct icmphdr)));

	printf("IP Hdr Dump:\n");

	printHeaderMemory(&msg, sizeof(struct iphdr));

	printf("ICMP: type %hu, code %hu, size %hu, id %#x, seq 0x%04x\n", 
			msg.icmp.icmphdr.type,
			msg.icmp.icmphdr.code,
			size,
			msg.icmp.icmphdr.un.echo.id,
			msg.icmp.icmphdr.un.echo.sequence
		  );
}

void printMessageStatistics() {
	struct iphdr iphdr;
	struct icmphdr icmphdr;
	char rslv_node[INET_ADDRSTRLEN];
	char *ip_str;

	iphdr = g_ping->pkt_msg.iphdr;

	icmphdr = g_ping->pkt_msg.icmp.icmphdr;

	ip_str = &rslv_node[0];
	ft_memset(ip_str, 0, INET_ADDRSTRLEN);
	if (inet_ntop(AF_INET, &iphdr.saddr, ip_str, INET_ADDRSTRLEN) == NULL) {
		printError("ERROR: ntop");
		cleanPing();
		exit(FAILURE);
	}

	dprintf(STDOUT_FILENO, "%lu bytes from %s: icmp_seq=%d ttl=%d time=%.3lf ms\n",
			g_ping->msg_ret - sizeof(struct iphdr), ip_str, icmphdr.un.echo.sequence, iphdr.ttl, g_ping->time.diff_ms);
}

void setMsgPointer() {
	g_ping->pkt_msg = *(t_msg_packet*)(&g_ping->databuf);
}

void setupReception() {
	struct msghdr *msg;

	msg = &g_ping->msg;
	ft_memset(msg, 0, sizeof(*msg));

    msg->msg_iov = g_ping->iov;
    msg->msg_iovlen = 1;

    msg->msg_iov[0].iov_base = g_ping->databuf;
    msg->msg_iov[0].iov_len = sizeof(g_ping->databuf);


    msg->msg_control = &g_ping->control;
    msg->msg_controllen = sizeof(g_ping->control);

    msg->msg_flags = 0;
	g_ping->rec_flags = MSG_DONTWAIT;
}

int isValidReply() {
	t_msg_packet pkt;
	uint32_t addr;
	uint16_t id;
	uint16_t checksum;

	pkt = *(t_msg_packet*)(&g_ping->databuf);

	addr = g_ping->destination.integer;
	id = g_ping->pid;
	checksum = pkt.icmp.icmphdr.checksum;

	if ((pkt.iphdr.saddr == addr || addr == 0) 
			&& pkt.icmp.icmphdr.type == 0
			&& pkt.icmp.icmphdr.code == 0
			&& pkt.icmp.icmphdr.un.echo.sequence == g_ping->counters.recieved
			&& pkt.icmp.icmphdr.un.echo.id == id) {

		pkt.icmp.icmphdr.checksum = 0;
		if (icmpChecksum(&(pkt.icmp), sizeof(pkt.icmp)) == checksum) {
			return TRUE;
		}
	}

	return FALSE;
}

int hasReply() {
	if (g_ping->msg_ret == -1 && errno == EWOULDBLOCK) {
		return FALSE;
	}
	return TRUE;
}

void recieveMessage( ) {
	g_ping->msg_ret = recvmsg(g_ping->socket.sockfd, &g_ping->msg, g_ping->rec_flags);
	if (g_ping->msg_ret == -1 && errno != EWOULDBLOCK) {
		printError("ERROR: %s | Error reading msg\n", strerror(errno));
		cleanPing();
		exit(FAILURE);
	}
}

void handleValidReply() {
	// a msg was found and is the response we expect to parse

	// give us another TIMEOUT time to work
	setTimeoutAlarm();

	setMsgPointer();

	setRoudTripTime();

	setRecieved();

	updateStatistics();

	printMessageStatistics();
}

void handleInvalidReply() {
	t_err_msg_packet msg;
	// char ip_buff[INET_ADDRSTRLEN];
	// char hostname[NI_MAXHOST];
	// char *ip_str = &ip_buff[0];

	msg = *(t_err_msg_packet*)g_ping->databuf;
	printf("%lu bytes from ", msg.iphdr.tot_len - sizeof(msg.iphdr));

	// Reverse dns on 
	(void)msg.iphdr.saddr;
	// if (SUCCESS == 1) {
	// 			printf("%s (%s): ", hostname, ip_str);
	// }
	// else {
	// 			printf("%s: ", ip_str);
	// }

	// Check for error type messages
	if (msg.icmphdr.type == ICMP_DEST_UNREACH) {
		printf("Destination Host Unreachable");
	}
	else if (msg.icmphdr.type == ICMP_TIME_EXCEEDED) {
		printf("Time to live exceeded");
	}
	printf("\n");


	//Verbose add iphdr dump + icmp info of recieved message packt
	if ((g_ping->options & VERBOSE_OPTION) != 0) {
		printErrorMsgDump();
	}

}

// TODO
// ICMP_REDIRECT
// unknown for now
