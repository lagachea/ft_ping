#include "ft_ping.h"

void freePing() {
	freeaddrinfo(g_ping->results);
	g_ping->results = NULL;
	close(g_ping->socket.sockfd);
	g_ping->socket.sockfd = -1;
}

void	interruptHandler(int signal) {
	(void)signal;
	printf( "\n--- %s ping statistics ---\n", g_ping->canonname);
	printf("%u packets transmitted, %u received, %u%% packet loss, time %ums\n", g_ping->counters.transmitted, g_ping->counters.recieved, g_ping->counters.loss, g_ping->counters.sumtime);
	printf("rtt min/avg/max/mdev = %.3lf/%.3lf/%.3lf/%.3lf ms\n", g_ping->counters.min, g_ping->counters.avg, g_ping->counters.max, g_ping->counters.mdev);

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
