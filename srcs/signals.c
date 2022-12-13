#include "ft_ping.h"

void freePing() {
	freeaddrinfo(g_ping->results);
	g_ping->results = NULL;
	close(g_ping->socket.sockfd);
	g_ping->socket.sockfd = -1;
}

void	interruptHandler(int signal) {
	(void)signal;
	dprintf(STDERR_FILENO, "\nStopping gracefully\n");
	// printf( "--- %s ping statistics ---", );
	// printf("%d packets transmitted, %d received, %d%% packet loss, time 3005ms", transmitted, recieved, loss, sumtime);
	// printf("rtt min/avg/max/mdev = %.3lf/%.3lf/%.3lf/%.3lf ms", min, avg, max, mdev);

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
