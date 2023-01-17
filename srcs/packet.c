#include "ft_ping.h"

void setupInput() {
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
	g_ping->rec_flags = 0;
}

void getAddressInformation() {
	int res;
	struct addrinfo **ai_res;

	ai_res = &g_ping->results;
	if (*ai_res != NULL) {
		freeaddrinfo(*ai_res);
	}
	res = getaddrinfo(g_ping->node, g_ping->service, &g_ping->hints, ai_res);
	if (res < 0) {
		printError("ERROR: %s | Error getting addrinfo\n", gai_strerror(res));
		exit(FAILURE);
	}
}

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

void fillIcmp() {
	ft_memset(&g_ping->icmp, 0, sizeof(g_ping->icmp));
	g_ping->icmp.icmp_type = ICMP_ECHO;
	g_ping->icmp.icmp_code = 0;
	g_ping->icmp.icmp_id = g_ping->pid;
	g_ping->seq++;
	g_ping->icmp.icmp_seq = g_ping->seq;
	g_ping->icmp.icmp_cksum = icmpChecksum();
}

void setAddress() {
	struct in_addr addr_in;

	getAddressInformation();
	g_ping->dest_addr = *g_ping->results->ai_addr;
	g_ping->canonname = g_ping->results->ai_canonname;

	// ADDRESS as BYTES
	addr_in = ((struct sockaddr_in*)(&g_ping->dest_addr))->sin_addr;
	inet_ntop(AF_INET, &addr_in, g_ping->ip_str, INET_ADDRSTRLEN);
}

void setClock(struct timeval *tv) {
	int ret;

	ft_memset(tv, 0, sizeof(*tv));
	ret = gettimeofday(tv, NULL);
	if (ret != 0) {
		printError("ERROR: %s | Failed setting clock\n", strerror(errno));
		freePing();
		exit(FAILURE);
	}
}

void setOriginalClock() {
	ft_memcpy(&g_ping->time.tvo, &g_ping->time.tvi, sizeof(struct timeval));
}

void setInitialClock() {
	setClock(&g_ping->time.tvi);
	if (g_ping->seq == 0) {
		setOriginalClock();
	}
}

void setFinalClock() {
	setClock(&g_ping->time.tvf);
}

void getTimeDiff() {
	t_clock *t;

	setFinalClock();
	t = &g_ping->time;
	t->diff = getDiff(&t->tvf, &t->tvi);
	t->diff_ms = t->diff / 1000.0;
}

unsigned int getDiff(struct timeval *tvf, struct timeval *tvi) {
	long int diff;
	long int diff_sec;
	long int diff_usec;
	diff_sec = tvf->tv_sec - tvi->tv_sec;
	diff_usec = tvf->tv_usec - tvi->tv_usec;
	diff = diff_sec * 1000000 + diff_usec;
	return diff;
}

void printInitialInformation() {
	/* if hostname */
	printf("PING %s (%s) %d(%d) bytes of data.\n", g_ping->canonname, g_ping->ip_str, ICMP_MINLEN, PACKET_LEN);
	/* else ip */
	// printf("PING %s (%s) %d(%d) bytes of data.\n", g_ping->canonname, g_ping->ip_str, 8, 8);
}

void setupOutput() {
	setInitialClock();
	setupInput();
	/* if hostname */
	if (g_ping->state == HOSTNAME) {
		setAddress();
	}

	if (g_ping->seq == 0) {
		printInitialInformation();
	}

	fillIcmp();
}
