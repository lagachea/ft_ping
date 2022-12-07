#include "ft_ping.h"
#include "libft.h"
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stddef.h>
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

	if (g_ping->results != NULL) {
		freeaddrinfo(g_ping->results);
	}
	res = getaddrinfo(g_ping->node, g_ping->service, &(g_ping->hints), &(g_ping->results));
	if (res < 0) {
		printf("%s\n", gai_strerror(res));
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
	g_ping->icmp.icmp_hun.ih_idseq.icd_id = g_ping->pid;
	g_ping->icmp.icmp_hun.ih_idseq.icd_seq = g_ping->seq;
	g_ping->icmp.icmp_cksum = icmpChecksum();
	g_ping->seq++;
}

void setAddr() {
	g_ping->dest_addr = *g_ping->results->ai_addr;
	g_ping->addrlen = g_ping->results->ai_addrlen;
}

void setupOutput() {
<<<<<<< HEAD
	setHandler(SIGINT, &interruptHandler);
=======
>>>>>>> 7a438df (set handler set alarm)
	setupInput();
	getAInfo();
	fillIcmp();
	setAddr();
}
