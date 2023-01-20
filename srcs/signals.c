#include "ft_ping.h"

void freePing() {
	freeaddrinfo(g_ping->results);
	g_ping->results = NULL;
	close(g_ping->socket.sockfd);
	g_ping->socket.sockfd = -1;
}

void	sigquitHandler(int signal) {
	(void)signal;
	printShortStatistics();
}

void	interruptHandler(int signal) {
	(void)signal;
	printStatistics();
	freePing();
	exit(0);
}

void	timeoutHandler(int signum) {
	(void)signum;
	// Alarm timeout after sending packet
	dprintf(STDERR_FILENO ,"timeout detected\n");
	printStatistics();
	freePing();
	exit(FAILURE);
}

void setHandler(int signum, sighandler_t handler) {
	signal(signum, handler);
}

void setAlarmHandler(int sec, sighandler_t handler) {
	alarm(0);
	setHandler(SIGALRM, handler);
	alarm(sec);
}
