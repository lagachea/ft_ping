#include "ft_ping.h"

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
