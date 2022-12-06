#include "ft_ping.h"
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>

static void DumpHex(void *address, int size) {
	int count;
	for (count = 0; count < size; count++){
		printf("%.2x ", ((unsigned char*)address)[count]);
	}
	printf("\n");
}

static void icmpChecksum() {
	uint16_t res;
	uint16_t *ptr;
	size_t count;

	res = 0;
	ptr = (uint16_t *)&g_ping->icmp;
	count = 0;
	while (count < 8) {
		res += *ptr;
		ptr++;
		count++;
	}
	g_ping->icmp.icmp_cksum = ~res;
}

void fillIcmp() {
	g_ping->icmp.icmp_type = ICMP_ECHO;
	g_ping->icmp.icmp_code = 0;
	g_ping->icmp.icmp_hun.ih_idseq.icd_id = g_ping->pid;
	g_ping->icmp.icmp_hun.ih_idseq.icd_seq = g_ping->seq;
	icmpChecksum();
	g_ping->seq++;
}

void setupRecv() {
	struct msghdr *msghdrptr;

	msghdrptr = &g_ping->msg;
	msghdrptr->msg_name = &g_ping->sin;
	msghdrptr->msg_namelen = sizeof(g_ping->sin);

    msghdrptr->msg_iov = g_ping->iov;
    msghdrptr->msg_iovlen = 1;

    msghdrptr->msg_iov[0].iov_base = g_ping->databuf;
    msghdrptr->msg_iov[0].iov_len = sizeof(g_ping->databuf);


    msghdrptr->msg_control = &g_ping->control;
    msghdrptr->msg_controllen = sizeof(g_ping->control);

    msghdrptr->msg_flags = 0;


	g_ping->rec_flags = 0;
	printf("set rcv\n");
}
void printMsg(int len) {
	printf("namelen= %d\n", g_ping->msg.msg_namelen);
	printf("controllen= %zu\n", g_ping->msg.msg_controllen);
	printf("iovlen= %zu\n", g_ping->msg.msg_iovlen);

	printf("read %d\n", len);
	DumpHex(g_ping->databuf, len);

	if (g_ping->msg.msg_namelen > 0)
	{
		DumpHex(g_ping->msg.msg_name, g_ping->msg.msg_namelen);
	}
	if (g_ping->msg.msg_controllen > 0)
	{
		DumpHex(g_ping->msg.msg_control, g_ping->msg.msg_controllen);
	}
	if (g_ping->msg.msg_iovlen > 0)
	{
		size_t count = 0;
		while (count < g_ping->msg.msg_iovlen) {
			DumpHex(g_ping->msg.msg_iov[count].iov_base, g_ping->msg.msg_iov[count].iov_len);
			count++;
		}
	}
}
