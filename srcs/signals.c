#include "ft_ping.h"

void freePing() {
	freeaddrinfo(g_ping->results);
	g_ping->results = NULL;
	close(g_ping->socket.sockfd);
	g_ping->socket.sockfd = -1;
}

void	interruptHandler(int signal) {
	(void)signal;
	printStatistics();
	freePing();
	exit(0);
}

void	loopHandler(int signum) {
	(void)signum;
	looping();
}

void	timeoutHandler(int signum) {
	(void)signum;
	// Alarm timeout after sending packet
	dprintf(STDERR_FILENO ,"timeout detected\n");
	printStatistics();
	freePing();
	exit(1);
}

void setHandler(int signum, sighandler_t handler) {
	signal(signum, handler);
}

void setAlarmHandler(int sec, sighandler_t handler) {
	alarm(0);
	setHandler(SIGALRM, handler);
	alarm(sec);
}
