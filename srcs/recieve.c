#include "ft_ping.h"
#include <stdio.h>

void printMsg(int len) {
	struct icmp *icmptr;
	struct ip *ipptr;
	t_clock *t;

	// printf("read %d\n", len);

	ipptr = (void*)g_ping->databuf;
	// printIp(ipptr);

	icmptr = (void*)g_ping->databuf + 20;
	// printIcmp(icmptr);

	t = &g_ping->time;
	getTimeDiff();
	set_recieved();
	update_stats();
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.1lf ms\n", len, g_ping->ip_str, icmptr->icmp_seq, ipptr->ip_ttl, t->diff_ms);
	return;
	// printf("namelen= %d\n", g_ping->msg.msg_namelen);
	// printf("controllen= %zu\n", g_ping->msg.msg_controllen);
	// printf("iovlen= %zu\n", g_ping->msg.msg_iovlen);

	// if (g_ping->msg.msg_namelen > 0)
	// {
	// 	print_memory(g_ping->msg.msg_name, g_ping->msg.msg_namelen);
	// }
	// if (g_ping->msg.msg_controllen > 0)
	// {
	// 	print_memory(g_ping->msg.msg_control, g_ping->msg.msg_controllen);
	// }
	// if (g_ping->msg.msg_iovlen > 0)
	// {
	// 	size_t count = 0;
	// 	while (count < g_ping->msg.msg_iovlen) {
	// 		print_memory(g_ping->msg.msg_iov[count].iov_base, g_ping->msg.msg_iov[count].iov_len);
	// 		count++;
	// 	}
	// }
}

void recieveMsg( ) {
	int res = 0;

	res = recvmsg(g_ping->socket.sockfd, &g_ping->msg, g_ping->rec_flags);
	if (res == -1) {
		printError("ERROR:%s | Error reading msg\n", strerror(errno));
		freePing();
		exit(FAILURE);
	}
	else if (res >= 0) {
		alarm(0);
		printMsg(res);
		return ;
	}
}
