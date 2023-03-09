#include "ft_ping.h"

static uint16_t icmpChecksum(void *pkt, int len) {
	uint32_t sum;
	uint16_t *ptr;
	int count;

	ptr = (uint16_t *)pkt;
	sum = 0;
	count = 0;

	// sum all uint16 of the packet in a uint32
	while (count < len) {
		sum += *ptr;
		ptr++;
		count += sizeof(uint16_t);
	}

	// sum 16 highest bits with 16 lowest bits
	// until no more bits are set in the highest 16
	while (sum & 0xFFFF0000) {
		sum = (sum >> 16) + (sum & 0xFFFF);
	}

	// return 1's complement of sum
	return ~sum;
}

static void fillIcmp() {
	t_icmp_out *icmp_out;

	icmp_out = &g_ping->pkt_msg.icmp;
	ft_memset(icmp_out, 0, sizeof(t_icmp_out));
	icmp_out->icmphdr.type = ICMP_ECHO;
	icmp_out->icmphdr.code = 0;
	icmp_out->icmphdr.un.echo.id = g_ping->pid;
	icmp_out->icmphdr.un.echo.sequence = g_ping->seq;
	//Set timestamp in icmp_out->data
	ft_memcpy(&icmp_out->data[0], &g_ping->time.emission, sizeof(struct timeval));

	icmp_out->icmphdr.checksum = icmpChecksum(icmp_out, sizeof(t_icmp_out));
	g_ping->seq++;
}

void setClock(struct timeval *tv) {
	int ret;

	ft_memset(tv, 0, sizeof(*tv));
	ret = gettimeofday(tv, NULL);
	if (ret != 0) {
		printError("ERROR: %s | Failed setting clock\n", strerror(errno));
		cleanPing();
		exit(FAILURE);
	}
}

void setEmissionClock() {
	setClock(&g_ping->time.emission);
}

void setReceptionClock() {
	setClock(&g_ping->time.reception);
}

void setWaitClock() {
	setClock(&g_ping->time.wait);
}

void setMsgClock() {
	g_ping->time.msg = *(struct timeval*)(&g_ping->pkt_msg.icmp.data[0]);
}

void setRoudTripTime() {
	// other goal is to use long int for stats
	t_clock *t;

	t = &g_ping->time;
	setMsgClock();
	setReceptionClock();
	t->diff = getTimeDiff(&t->reception, &t->msg);
	t->diff_ms = t->diff / 1000.0;
}

long int getTimeDiff(struct timeval *tvf, struct timeval *tvi) {
	long int diff;
	long int diff_sec;
	long int diff_usec;
	diff_sec = tvf->tv_sec - tvi->tv_sec;
	diff_usec = tvf->tv_usec - tvi->tv_usec;
	diff = diff_sec * ONE_SEC + diff_usec;
	return diff;
}

void printInitialInformation() {

	/* if verbose add identifier */
	if ((g_ping->options & VERBOSE_OPTION) != 0) {
		dprintf(STDOUT_FILENO, "PING %s (%s): %d data bytes, id 0x%04x = %d\n", g_ping->canonname, g_ping->ip_str, ICMP_DATA_LEN, g_ping->pid, g_ping->pid);
	}
	else {
		dprintf(STDOUT_FILENO, "PING %s (%s): %d data bytes\n", g_ping->canonname, g_ping->ip_str, ICMP_DATA_LEN);
	}
}

void setupRoundTrip() {
	setEmissionClock();
	fillIcmp();
}

void sendPing() {
	int res;

	res = sendto(g_ping->socket.sockfd, &g_ping->pkt_msg.icmp, ICMP_FULL, 0, &g_ping->dest_addr, INET_ADDRSTRLEN);
	if (res == -1) {
		printError("ERROR: %s | sending packet to %s\n", strerror(errno), g_ping->ip_str);
		cleanPing();
		exit(FAILURE);
	}
}
