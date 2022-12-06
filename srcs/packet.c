#include "ft_ping.h"
#include "libft.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>

void setupInput() {
	struct msghdr *msg;

	msg = &g_ping->msg;
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


static void icmpChecksum() {
	uint16_t res;
	uint16_t *ptr;
	size_t count;

	res = 0;
	g_ping->icmp.icmp_cksum = 0;
	ptr = (uint16_t *)&g_ping->icmp;
	count = 0;
	while (count < 8) {
		res += *ptr;
		ptr++;
		count++;
	}
	g_ping->icmp.icmp_cksum = ~res;
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

void fillIcmp() {
	g_ping->icmp.icmp_type = ICMP_ECHO;
	g_ping->icmp.icmp_code = 0;
	g_ping->icmp.icmp_hun.ih_idseq.icd_id = g_ping->pid;
	g_ping->icmp.icmp_hun.ih_idseq.icd_seq = g_ping->seq;
	icmpChecksum();
	g_ping->seq++;
}

void setAddr() {
	g_ping->dest_addr = *g_ping->results->ai_addr;
	g_ping->addrlen = g_ping->results->ai_addrlen;
}

void setupOutput() {
	getAInfo();
	fillIcmp();
	setAddr();
}
void printMsg(int len) {
	// printf("namelen= %d\n", g_ping->msg.msg_namelen);
	// printf("controllen= %zu\n", g_ping->msg.msg_controllen);
	// printf("iovlen= %zu\n", g_ping->msg.msg_iovlen);

	// printf("read %d\n", len);
	print_memory(g_ping->databuf, len);

	return;
	if (g_ping->msg.msg_namelen > 0)
	{
		print_memory(g_ping->msg.msg_name, g_ping->msg.msg_namelen);
	}
	if (g_ping->msg.msg_controllen > 0)
	{
		print_memory(g_ping->msg.msg_control, g_ping->msg.msg_controllen);
	}
	if (g_ping->msg.msg_iovlen > 0)
	{
		size_t count = 0;
		while (count < g_ping->msg.msg_iovlen) {
			print_memory(g_ping->msg.msg_iov[count].iov_base, g_ping->msg.msg_iov[count].iov_len);
			count++;
		}
	}
}
