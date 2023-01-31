#include "ft_ping.h"

static uint16_t icmpChecksum() {
	uint16_t res;
	uint16_t *ptr;
	size_t count;

	ptr = (uint16_t *)&g_ping->icmp;
	res = 0;
	count = 0;
	while (count < 8) {
		res += *ptr;
		ptr++;
		count++;
	}
	return ~res;
}

static void fillIcmp() {
	ft_memset(&g_ping->icmp, 0, sizeof(g_ping->icmp));
	g_ping->icmp.type = ICMP_ECHO;
	g_ping->icmp.code = 0;
	g_ping->icmp.un.echo.id = g_ping->pid;
	g_ping->seq++;
	g_ping->icmp.un.echo.sequence = g_ping->seq;
	g_ping->icmp.checksum = icmpChecksum();
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
		printf("PING %s (%s) %d data bytes, id 0x%x = %d\n", g_ping->canonname, g_ping->ip_str, ICMP_MINLEN, g_ping->pid, g_ping->pid);
	}
	else {
		printf("PING %s (%s) %d data bytes\n", g_ping->canonname, g_ping->ip_str, ICMP_MINLEN);
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

	res = sendto(g_ping->socket.sockfd, &g_ping->icmp, ICMP_MINLEN, 0, &g_ping->dest_addr, INET_ADDRSTRLEN);
	if (res == -1) {
		printError("Error:%s | sending packet to %s\n", strerror(errno), g_ping->dest_addr.sa_data);
		cleanPing();
		exit(FAILURE);
	}
}
