#include "ft_ping.h"

void printMessageStatistics(int len) {
	struct icmp *icmptr;
	struct ip *ipptr;

	ipptr = (struct ip*)g_ping->databuf;

	icmptr = (struct icmp*)g_ping->databuf + 20;

	getTimeDiff();
	setRecieved();
	updateStatistics();

	/* if ip */
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.1lf ms\n",
			len, g_ping->ip_str, ntohs(icmptr->icmp_seq), ipptr->ip_ttl, g_ping->time.diff_ms);
	/* if hostname */
	// printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.1lf ms\n",
	// 		len, g_ping->ip_str, g_ping->ip_str, ntohs(icmptr->icmp_seq), ipptr->ip_ttl, t->diff_ms);
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

void recieveMessage( ) {
	int res = 0;

	res = recvmsg(g_ping->socket.sockfd, &g_ping->msg, g_ping->rec_flags);
	if (res == -1) {
		printError("ERROR:%s | Error reading msg\n", strerror(errno));
		freePing();
		exit(FAILURE);
	}
	else if (res >= 0) {
		alarm(0);
		printMessageStatistics(res);
		return ;
	}
}
