#include "ft_ping.h"

void setup() {
	ft_memset(g_ping, 0, sizeof(t_ftping));

	setHandler(SIGINT, &interruptHandler);
	setHandler(SIGQUIT, &sigquitHandler);
	setHandler(SIGALRM, &timeoutHandler);

	g_ping->service = NULL;
	g_ping->pid = getpid();

	g_ping->hints.ai_flags = AI_CANONNAME;
	g_ping->hints.ai_family = AF_INET;

	g_ping->ip_str = &g_ping->rslv_node[0];
	g_ping->canonname = &g_ping->hostname[0]; 
}

int ft_strcountchr(char *str, int c) {
	int count = 0;
	while(*str) {
		if (c == *str) {
			count++;
		}
		str++;
	}
	return count;
}

void	printMemory(const void *addr, size_t size)
{
	unsigned char *t = (unsigned char *)addr;
	char		print;
	size_t		i = 0;
	size_t		col;
	size_t		tmp = 0;
	size_t		cut = 16;

	while (i < size)
	{
		col = 0;
		tmp = i;
		while (col < cut)
		{
			if (i < size)
				printf("%-4.2x", t[i]);
			else
				printf("    ");
			i++;
			col++;
		}
		col = 0;
		i = tmp;
		while (col < cut && i < size) {
			unsigned char c;
			c = t[i++];
			// print = (c > 31 && c < 127) ? c : '.';
			print = '.';
			if (ft_isprint(c)) {
				print = c;
			}
			printf("%c", print);
			col++;
		}
		printf("\n");
	}
}

void printIp(struct iphdr *ipptr) {
	printf("ip\n");
	printMemory(ipptr, 20);
}
void printIcmp(struct icmphdr *icmptr) {
	printf("icmp\n");
	printMemory(icmptr, 8);
}

void printStatistics() {
	setLoss();
	dprintf(STDOUT_FILENO,  "--- %s ping statistics ---\n",
			g_ping->canonname);
	dprintf(STDOUT_FILENO, "%ld packets transmitted, %ld packets received, %ld%% packet loss\n",
			g_ping->counters.transmitted,
			g_ping->counters.recieved,
			g_ping->counters.loss_percent);
	if (g_ping->counters.recieved > 0) {
		dprintf(STDOUT_FILENO, "round-trip min/avg/max/stddev = %.3lf/%.3lf/%.3lf/%.3lf ms\n",
				g_ping->counters.min,
				g_ping->counters.avg,
				g_ping->counters.max,
				g_ping->counters.mdev);
	}
}

void printShortStatistics() {
	setLoss();
	dprintf(STDOUT_FILENO, "\r%ld/%ld packets, %ld%% loss\n",
			g_ping->counters.recieved,
			g_ping->counters.transmitted,
			g_ping->counters.loss_percent);
}

void printTimeval(struct timeval *tv) {
	unsigned int t;

	t = tv->tv_sec * ONE_SEC + tv->tv_usec;
	dprintf(STDOUT_FILENO, "%u\n", t);
}

void printUsage() {
	dprintf(STDERR_FILENO, "./ft_ping [-vh] destination\n");
}
