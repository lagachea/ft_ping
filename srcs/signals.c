#include "ft_ping.h"

void cleanPing() {
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
	cleanPing();
	exit(SUCCESS);
}

void	timeoutHandler(int signum) {
	(void)signum;
	printError("timeout detected\n");
	printStatistics();
	cleanPing();
	exit(FAILURE);
}

void setHandler(int signum, sighandler_t handler) {
	signal(signum, handler);
}

void setTimeoutAlarm() {
	alarm(TIMEOUT);
}

void resetAlarm() {
	alarm(0);
	setTimeoutAlarm();
}
