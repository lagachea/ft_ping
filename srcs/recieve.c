#include "ft_ping.h"

int getIPHeaderLengthInBytes(struct iphdr *ipptr) {
	/* IP header encode the number of 32 bits packet there is in ip_hl field */
	/* as 32 bits is 4 bytes the ip_hl * (32 / 8) can be reduced to  */
	/* ip_hl * 4 */
	return ipptr->ihl * 4;
}

void printErrorMsgDump(t_hdr_packet *request) {

	printf("IP Hdr Dump:\n");

	printHeaderMemory(&request->iphdr, sizeof(struct iphdr));

	uint16_t size = g_ping->msg_ret - sizeof(struct iphdr) - sizeof(struct icmphdr) - sizeof(struct iphdr);
	printf("ICMP: type %hu, code %hu, size %hu, id %#x, seq 0x%04x\n", 
			request->icmphdr.type,
			request->icmphdr.code,
			size,
			request->icmphdr.un.echo.id,
			request->icmphdr.un.echo.sequence
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

void printErrorMessage(struct icmphdr *reply) {
	char *message = NULL;
	t_err_msg err_messages[] = {
		{ICMP_DEST_UNREACH, ICMP_NET_UNREACH, "Destination Network unreachable"},
		{ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, "Destination Host unreachable"},
		{ICMP_DEST_UNREACH, ICMP_PROT_UNREACH, "Destination Protocol unreachable"},
		{ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, "Destination Port unreachable"},
		{ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, "Fragmentation needed/DF flag set"},
		{ICMP_DEST_UNREACH, ICMP_SR_FAILED, "Source Route failed"},
		{ICMP_DEST_UNREACH, ICMP_NET_UNKNOWN, "Unknown network"},
		{ICMP_DEST_UNREACH, ICMP_HOST_UNKNOWN, "Unknown host"},
		{ICMP_DEST_UNREACH, ICMP_HOST_ISOLATED, "Host isolated"},
		{ICMP_DEST_UNREACH, ICMP_NET_ANO, "Network administratively prohibited"},
		{ICMP_DEST_UNREACH, ICMP_HOST_ANO, "Host administratively prohibited"},
		{ICMP_DEST_UNREACH, ICMP_NET_UNR_TOS, "Network unreachable for ToS"},
		{ICMP_DEST_UNREACH, ICMP_HOST_UNR_TOS, "Host unreachable for ToS"},
		{ICMP_DEST_UNREACH, ICMP_PKT_FILTERED, "Packet filtered"},
		{ICMP_DEST_UNREACH, ICMP_PREC_VIOLATION, "Host Precedence Violation"},
		{ICMP_DEST_UNREACH, ICMP_PREC_CUTOFF, "Precedence cutoff in effect"},
		{ICMP_REDIRECT, ICMP_REDIR_NET, "Redirect Datagram for the Network"},
		{ICMP_REDIRECT, ICMP_REDIR_HOST, "Redirect Datagram for the Host"},
		{ICMP_REDIRECT, ICMP_REDIR_NETTOS, "Redirect Datagram for the ToS & network"},
		{ICMP_REDIRECT, ICMP_REDIR_HOSTTOS, "Redirect Datagram for the ToS & host"},
		{ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, "Time to live exceeded"},
		{ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, "Fragment reassembly time exceeded"},
		{-1, -1, "NO"}
	};
	t_err_msg *err_ptr = &err_messages[0];

	while (err_ptr->type != -1) {
		if (reply->type == err_ptr->type && reply->code == err_ptr->code)
		{
		  message = err_ptr->message;
		}
		err_ptr++;
	}
	if (message != NULL) {
		printf("%s\n", message);
		return;
	}

	// ERROR FILTER IS NOT WORKING

}

void handleInvalidReply() {
	int ret;
	t_hdr_packet reply;
	t_hdr_packet request;
	struct sockaddr_in sock_addr;

	char ip_buff[INET_ADDRSTRLEN];
	char hostname[NI_MAXHOST];
	char *ip_str;

	reply = *(t_hdr_packet*)g_ping->databuf;
	request = *(t_hdr_packet*)(g_ping->databuf + sizeof(t_hdr_packet));

	if (request.icmphdr.un.echo.id != g_ping->pid) {
		return;
	}

	printf("%lu bytes from ", reply.iphdr.tot_len - sizeof(reply.iphdr));

	ip_str = &ip_buff[0];
	ft_memset(ip_str, 0, INET_ADDRSTRLEN);
	if (inet_ntop(AF_INET, &reply.iphdr.saddr, ip_str, INET_ADDRSTRLEN) == NULL) {
		printError("ERROR: ntop");
		cleanPing();
		exit(FAILURE);
	}
	
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = reply.iphdr.saddr;
	ret	= reverseDNSquery((struct sockaddr*)&sock_addr, &hostname[0]);
	if (ret == SUCCESS) {
		printf("%s (%s): ", hostname, ip_str);
	}
	else {
		printf("%s: ", ip_str);
	}

	printErrorMessage(&reply.icmphdr);


	//Verbose add iphdr dump + icmp info of recieved message packt
	if ((g_ping->options & VERBOSE_OPTION) != 0) {
		printErrorMsgDump(&request);
	}

}

// TODO
// ICMP_REDIRECT
// unknown for now
