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
	uint8_t *t = (unsigned char *)addr;
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
			uint8_t c;
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

void printBytes(const void* addr, size_t size) {
	uint8_t *t = (uint8_t *)addr;
	size_t		i = 0;

	while (i < size) {
		printf("%02x", t[i]);
		i++;
	}
}

void	printHeaderMemory(struct iphdr *addr, size_t size)
{
	uint8_t *t = (uint8_t *)addr;
	size_t		i = 0;
	struct iphdr iphdr = *addr;

	while (i < size) {
		printf("%s%02x", i % 2 == 0 ? " " : "", t[i]);
		i++;
	}
	printf("\n");

	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");

	// Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data
	// 4  5  00 0054 9bc8   2 0000  01  01 6b4c 172.21.196.125  1.1.1.1 

	printf(" %1x", iphdr.version);

	printf("  %1x", iphdr.ihl);

	printf("  %02x", iphdr.tos);

	printf(" ");
	printBytes(&iphdr.tot_len, sizeof(iphdr.tot_len));

	printf(" ");
	printBytes(&iphdr.id, sizeof(iphdr.id));

	// Need ntohs for field > 8 bits
	printf("   %1x", (iphdr.frag_off & 0xE000u) >> 13); // 3 / 16 bits
	printf(" %04x", iphdr.frag_off & 0x1FFF); // 13 / 16 bits

	printf("  %02x", iphdr.ttl);

	printf("  %02x", iphdr.protocol);

	printf(" ");
	printBytes(&iphdr.check, sizeof(iphdr.check));

	char src[INET_ADDRSTRLEN];
	ft_memset(&src[0], 0, INET_ADDRSTRLEN);
	if (inet_ntop(AF_INET, &iphdr.saddr, &src[0], INET_ADDRSTRLEN) == NULL) {
		cleanPing();
		printError("ERROR: ntop");
		exit(FAILURE);
	}
	printf(" %s ", src);

	char dst[INET_ADDRSTRLEN];
	ft_memset(&dst[0], 0, INET_ADDRSTRLEN);
	if (inet_ntop(AF_INET, &iphdr.daddr, &dst[0], INET_ADDRSTRLEN) == NULL) {
		cleanPing();
		printError("ERROR: ntop");
		exit(FAILURE);
	}
	printf(" %s ", dst);

	printf("\n");
}

int reverseDNSquery(struct sockaddr *sock_addr, char *hostname) {
	int ret;

	ret = getnameinfo(sock_addr, sizeof(struct sockaddr),
			hostname, NI_MAXHOST, NULL, 0, NI_NAMEREQD);
	if (ret == 0) {
		return SUCCESS;
	}
	else if (ret == EAI_NONAME) {
		return NO_NAME;
	}
	cleanPing();
	exit(FAILURE);
}
