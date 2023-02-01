#include "ft_ping.h"

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
	printf( "--- %s ping statistics ---\n",
			g_ping->canonname);
	printf("%ld packets transmitted, %ld packets received, %ld%% packet loss\n",
			g_ping->counters.transmitted,
			g_ping->counters.recieved,
			g_ping->counters.loss_percent);
	if (g_ping->counters.recieved > 0) {
		printf("round-trip min/avg/max/stddev = %.3lf/%.3lf/%.3lf/%.3lf ms\n",
				g_ping->counters.min,
				g_ping->counters.avg,
				g_ping->counters.max,
				g_ping->counters.mdev);
	}
	else {
		printf("\n");
	}

}

void printShortStatistics() {
	setLoss();
	printf("\r%ld/%ld packets, %ld%% loss\n",
			g_ping->counters.recieved,
			g_ping->counters.transmitted,
			g_ping->counters.loss_percent);
}

void printTimeval(struct timeval *tv) {
	unsigned int t;

	t = tv->tv_sec * ONE_SEC + tv->tv_usec;
	printf("%u\n", t);
}

void printUsage() {
	printf("./ft_ping [-vh] destination\n");
}

int expectMessage() {
	if (g_ping->counters.transmitted > g_ping->counters.recieved) {
		return TRUE;
	}
	return FALSE;
}
