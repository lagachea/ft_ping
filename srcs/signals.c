#include "ft_ping.h"

void freePing() {
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
	printError("timeout detected\n");
	printStatistics();
	freePing();
	exit(FAILURE);
}

void setHandler(int signum, sighandler_t handler) {
	signal(signum, handler);
}

void setAlarm(int sec) {
	alarm(sec);
}

void resetAlarm() {
	alarm(0);
}
