#include "ft_ping.h"

void	print_memory(const void *addr, size_t size)
{
	unsigned char *t = (unsigned char *)addr;
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
			printf("%c", (c > 31 && c < 127) ? c : '.');
			col++;
		}
		printf("\n");
	}
}

void printIp(struct ip *ipptr) {
	printf("ip\n");
	print_memory(ipptr, 20);
}
void printIcmp(struct icmp *icmptr) {
	printf("icmp\n");
	print_memory(icmptr, 8);
}

void printStatistics() {
	printf( "\n--- %s ping statistics ---\n",
			g_ping->canonname);
	printf("%u packets transmitted, %u received, %u%% packet loss, time %ums\n",
			g_ping->counters.transmitted,
			g_ping->counters.recieved,
			g_ping->counters.loss_percent,
			g_ping->counters.sumtime);
	printf("rtt min/avg/max/mdev = %.3lf/%.3lf/%.3lf/%.3lf ms\n",
			g_ping->counters.min,
			g_ping->counters.avg,
			g_ping->counters.max,
			g_ping->counters.mdev);

}
