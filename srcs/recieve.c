#include "ft_ping.h"

void recieveMsg( ) {
	int count = 1;
	int res = 0;
	alarm(10);
	while(1) {
		printf("recvmsg %d\n", count++);
		res = recvmsg(g_ping->socket.sockfd, &g_ping->msg, g_ping->rec_flags);
		if (res == -1) {
			printf("Error reading msg\n");
		}
		else if (res > 0) {
			alarm(0);
			printMsg(res);
			return ;
		}
	}
}
