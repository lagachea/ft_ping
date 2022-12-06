#include "ft_ping.h"

void printMsg(int len) {
	struct icmp *icmptr;
	struct ip *ipptr;

	icmptr = (void*)g_ping->databuf + 20;
	ipptr = (void*)g_ping->databuf;

	printf("read %d\n", len);
	printf("ip\n");
	print_memory(ipptr, 20);
	printf("icmp\n");
	print_memory(icmptr, 8);

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
	alarm(10);
	while (res == 0) {
		res = recvmsg(g_ping->socket.sockfd, &g_ping->msg, g_ping->rec_flags);
		if (res == -1) {
			printf("Error reading msg\n");
			freePing();
			exit(FAILURE);
		}
		else if (res > 0) {
			alarm(0);
			printMsg(res);
			return ;
		}
	}
}
