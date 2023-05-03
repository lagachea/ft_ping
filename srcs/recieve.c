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
	// Check for error type messages
	// ICMP_DEST_UNREACH
	// reverse DNS for iphdr.saddr
	// printMessageError 
	//Verbose add iphdr dump + icmp info of recieved message packt
/*
92 bytes from DESKTOP-RTD1524 (192.168.1.116): Destination Host Unreachable
IP Hdr Dump:
 4500 0054 9e52 4000 3f01 6aa0 ac15 c47d c0a8 017b 
Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data
 4  5  00 0054 9e52   2 0000  3f  01 6aa0 172.21.196.125  192.168.1.123 
ICMP: type 8, code 0, size 64, id 0x4772, seq 0x0000
*/


	// ICMP_TIME_EXCEEDED
	// reverse DNS for iphdr.saddr
	// printMessageError 
	//Verbose add iphdr dump + icmp info of recieved message packt
/*
92 bytes from GEN8 (192.168.1.1): Time to live exceeded
IP Hdr Dump:
 4500 0054 9bc8 4000 0101 6b4c ac15 c47d 0101 0101 
Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data
 4  5  00 0054 9bc8   2 0000  01  01 6b4c 172.21.196.125  1.1.1.1 
ICMP: type 8, code 0, size 64, id 0x4771, seq 0x0000
*/

	if ((g_ping->options & VERBOSE_OPTION) != 0) {
		printErrorMsgDump();
	}
	//Reuse printMessageStatistics into a printMessageError
	// dprintf(STDOUT_FILENO, "%d bytes ERROR MSG\n", g_ping->msg_ret);

	// ICMP_REDIRECT
	// unknown for now
}
