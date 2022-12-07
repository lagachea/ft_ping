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
	// printf("Ctrl C handler\n");
	dprintf(STDERR_FILENO, "\nSIGINT RECIEVED\n");
	freePing();
	exit(0);
}

void	loopHandler(int signum) {
	(void)signum;

	// time for new send after recv msg
	// printf("1 sec loop handler\n");
	looping();
}

void	timeoutHandler(int signum) {
	(void)signum;
	// Alarm timeout after sending packet
	dprintf(STDERR_FILENO ,"timeout handler\n");
	freePing();
	exit(1);
}

void setHandler(int signum, sighandler_t handler) {
	signal(signum, handler);
}

void setAlarmHandler(int sec, sighandler_t handler) {
	setHandler(SIGALRM, handler);
	alarm(sec);
}
