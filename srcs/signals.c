#include "ft_ping.h"
#include <unistd.h>

void freePing() {
	freeaddrinfo(g_ping->results);
	g_ping->results = NULL;
	close(g_ping->socket.sockfd);
	g_ping->socket.sockfd = -1;
}

void	interruptHandler(int signal) {
	(void)signal;
	dprintf(STDERR_FILENO, "\nSIGINT RECIEVED\n");
	freePing();
	exit(0);
}

void	alarmHandler(int signal) {
	(void)signal;
	dprintf(STDERR_FILENO ,"\nSIGALRM RECIEVED\n");
	freePing();
	exit(1);
}

void setHandlers() {
	signal(SIGINT, &interruptHandler);
	signal(SIGALRM, &alarmHandler);
}
