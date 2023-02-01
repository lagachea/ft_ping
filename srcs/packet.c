#include "ft_ping.h"

static uint16_t icmpChecksum(struct icmphdr *hdr) {
	uint16_t res;
	uint16_t *ptr;
	size_t count;

	ptr = (uint16_t *)hdr;
	res = 0;
	count = 0;
	while (count < ICMPHDR_LEN) {
		res += *ptr;
		ptr++;
		count++;
	}
	return ~res;
}

static void fillIcmp() {
	t_icmp_out *icmp_out;

	icmp_out = &g_ping->pkt_msg.icmp;
	ft_memset(icmp_out, 0, sizeof(t_icmp_out));
	icmp_out->icmphdr.type = ICMP_ECHO;
	icmp_out->icmphdr.code = 0;
	icmp_out->icmphdr.un.echo.id = g_ping->pid;
	icmp_out->icmphdr.un.echo.sequence = g_ping->seq;
	icmp_out->icmphdr.checksum = icmpChecksum(&icmp_out->icmphdr);

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

void setOriginalClock() {
	ft_memcpy(&g_ping->time.original, &g_ping->time.emission, sizeof(struct timeval));
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

void getRoudTripTime() {
	t_clock *t;

	setReceptionClock();
	t = &g_ping->time;
	t->diff = getTimeDiff(&t->reception, &t->emission);
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

static void printInitialInformation() {

	/* if verbose add identifier */
	if ((g_ping->options & VERBOSE_OPTION) != 0) {
		printf("PING %s (%s) %d data bytes, id 0x%x = %d\n", g_ping->canonname, g_ping->ip_str, ICMP_DATA_LEN, g_ping->pid, g_ping->pid);
	}
	else {
		printf("PING %s (%s) %d data bytes\n", g_ping->canonname, g_ping->ip_str, ICMP_DATA_LEN);
	}
}

void setupRoundTrip() {
	setEmissionClock();

	if (g_ping->seq == 0) {
		setOriginalClock();
		printInitialInformation();
	}

	fillIcmp();
}

void sendPing() {
	int res;

	res = sendto(g_ping->socket.sockfd, &g_ping->pkt_msg.icmp, ICMP_FULL, 0, &g_ping->dest_addr, INET_ADDRSTRLEN);
	if (res == -1) {
		printError("Error:%s | sending packet to %s\n", strerror(errno), g_ping->dest_addr.sa_data);
		cleanPing();
		exit(FAILURE);
	}
}
