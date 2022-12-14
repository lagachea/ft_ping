#include "ft_ping.h"
#include "libft.h"
#include <netdb.h>
#include <stdio.h>

void setupInput() {
	struct msghdr *msg;

	msg = &g_ping->msg;
	ft_memset(msg, 0, sizeof(struct msghdr));
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

void getAInfo() {
	int res;
	struct addrinfo **ai_res;

	ai_res = &g_ping->results;
	if (*ai_res != NULL) {
		freeaddrinfo(*ai_res);
	}
	res = getaddrinfo(g_ping->node, g_ping->service, &g_ping->hints, ai_res);
	if (res < 0) {
		printf("%s\n", gai_strerror(res));
		exit(FAILURE);
	}
	// printAiInfo();
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
	g_ping->icmp.icmp_seq = g_ping->seq;
	/* g_ping->icmp.icmp_otime = ;
	g_ping->icmp.icmp_rtime = ;
	g_ping->icmp.icmp_ttime = ; */
	g_ping->icmp.icmp_cksum = icmpChecksum();
	g_ping->seq++;
}

char *getReverseStr() {
	return NULL;
	struct sockaddr_in *saddr_in;
	struct in_addr *addr_in;
	unsigned char *arr;
	unsigned char swap;
	char *reverse_str;
	uint32_t	addr;

	printf("%s reversed ", g_ping->ip_str);
	saddr_in = (struct sockaddr_in*)(&g_ping->dest_addr);
	addr_in = &saddr_in->sin_addr;
	addr = *(uint32_t *)&g_ping->addr_in;
	arr = (unsigned char *)&addr;
	swap = arr[3];
	arr[3] = arr[0];
	arr[0] = swap;
	swap = arr[2];
	arr[2] = arr[1];
	arr[1] = swap;
	addr_in = (struct in_addr *)&addr;
	inet_ntop(AF_INET, addr_in, g_ping->ip_str2, INET_ADDRSTRLEN);
	printf("%s\n", g_ping->ip_str2);
	reverse_str = ft_strjoin(g_ping->ip_str2, ".in-addr.arpa");
	if (reverse_str == NULL) {
		freePing();
		exit(1);
	}
	printf("%s\n", reverse_str);
	return reverse_str;
}

void setAddr() {
	struct sockaddr_in *saddr_in;
	struct in_addr *addr_in;

	g_ping->dest_addr = *g_ping->results->ai_addr;
	g_ping->addrlen = g_ping->results->ai_addrlen;

	//
	g_ping->canonname = g_ping->results->ai_canonname;

	// ADDRESS as BYTES
	saddr_in = (struct sockaddr_in*)(&g_ping->dest_addr);
	addr_in = &saddr_in->sin_addr;
	g_ping->addr_in = *addr_in;
	inet_ntop(AF_INET, addr_in, g_ping->ip_str, INET_ADDRSTRLEN);
	if (g_ping->seq == 1) {
		printf("PING %s (%s) %d(%d) bytes of data.\n", g_ping->canonname, g_ping->ip_str, 8, 8);
	}

	// char *reverse_str;
	//
	// reverse_str = getReverseStr();
	// int res;
	// res = getaddrinfo(reverse_str, NULL, &g_ping->hints, &g_ping->reverse);
	// if (res < 0) {
	// 	printf("%s\n", gai_strerror(res));
	// 	exit(FAILURE);
	// }

	// print_memory(addr_in, 4);
	// printf("%s\n", g_ping->ip_str);

}

void setClock(struct timeval *tv) {
	int ret;
	ret = gettimeofday(tv, NULL);
	if (ret != 0) {
		printf("Error setting clock\n");
		freePing();
		exit(1);
	}
}

void setInitialClock() {
	struct timeval *tv;

	ft_memset(&g_ping->time, 0, sizeof(t_clock));
	tv = &g_ping->time.tvi;
	setClock(tv);
	// printf("\nInitial usec = %li | sec = %li |%lf|\n", tv->tv_usec, tv->tv_sec, tv->tv_sec + (1.0/1000000) * tv->tv_usec);
}

void setFinalClock() {
	struct timeval *tv;

	tv = &g_ping->time.tvf;
	setClock(tv);
	// printf("\nFinal usec = %li | sec = %li |%lf|\n", tv->tv_usec, tv->tv_sec, tv->tv_sec + (1.0/1000000) * tv->tv_usec);
}

void getTimeDiff() {
	t_clock *t;
	struct timeval *tvf;
	struct timeval *tvi;

	setFinalClock();
	t = &g_ping->time;
	tvf = &t->tvf;
	tvi = &t->tvi;
	t->diff_sec = tvf->tv_sec - tvi->tv_sec;
	t->diff_usec = tvf->tv_usec - tvi->tv_usec;
	t->diff = t->diff_sec + (t->diff_usec / 1000000.0);
}

void setupOutput() {
	setupInput();
	getAInfo();
	setAddr();
	fillIcmp();
	setInitialClock();
}
