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
	ft_memcpy(&g_ping->err_messages, (t_err_msg[]){
		{ICMP_DEST_UNREACH, ICMP_NET_UNREACH, "Destination Network Unreachable"},
		{ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, "Destination Host Unreachable"},
		{ICMP_DEST_UNREACH, ICMP_PROT_UNREACH, "Destination Protocol Unreachable"},
		{ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, "Destination Port Unreachable"},
		{ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, "Fragmentation needed/DF flag set"},
		{ICMP_DEST_UNREACH, ICMP_SR_FAILED, "Source Route failed"},
		{ICMP_DEST_UNREACH, ICMP_NET_UNKNOWN, "Unknown network"},
		{ICMP_DEST_UNREACH, ICMP_HOST_UNKNOWN, "Unknown host"},
		{ICMP_DEST_UNREACH, ICMP_HOST_ISOLATED, "Host isolated"},
		{ICMP_DEST_UNREACH, ICMP_NET_ANO, "Network administratively prohibited"},
		{ICMP_DEST_UNREACH, ICMP_HOST_ANO, "Host administratively prohibited"},
		{ICMP_DEST_UNREACH, ICMP_NET_UNR_TOS, "Network unreachable for ToS"},
		{ICMP_DEST_UNREACH, ICMP_HOST_UNR_TOS, "Host unreachable for ToS"},
		{ICMP_DEST_UNREACH, ICMP_PKT_FILTERED, "Packet filtered"},
		{ICMP_DEST_UNREACH, ICMP_PREC_VIOLATION, "Host Precedence Violation"},
		{ICMP_DEST_UNREACH, ICMP_PREC_CUTOFF, "Precedence cutoff in effect"},
		{ICMP_REDIRECT, ICMP_REDIR_NET, "Redirect Datagram for the Network"},
		{ICMP_REDIRECT, ICMP_REDIR_HOST, "Redirect Datagram for the Host"},
		{ICMP_REDIRECT, ICMP_REDIR_NETTOS, "Redirect Datagram for the ToS & network"},
		{ICMP_REDIRECT, ICMP_REDIR_HOSTTOS, "Redirect Datagram for the ToS & host"},
		{ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, "Time to live exceeded"},
		{ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, "Fragment reassembly time exceeded"},
		{-1, -1, ""},
	}, sizeof(g_ping->err_messages));
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
				dprintf(STDOUT_FILENO, "%-4.2x", t[i]);
			else
				dprintf(STDOUT_FILENO, "    ");
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
			dprintf(STDOUT_FILENO, "%c", print);
			col++;
		}
		dprintf(STDOUT_FILENO, "\n");
	}
}

void printIp(struct iphdr *ipptr) {
	dprintf(STDOUT_FILENO, "ip\n");
	printMemory(ipptr, 20);
}
void printIcmp(struct icmphdr *icmptr) {
	dprintf(STDOUT_FILENO, "icmp\n");
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
		dprintf(STDOUT_FILENO, "%02x", t[i]);
		i++;
	}
}

void	printHeaderMemory(struct iphdr *addr, size_t size)
{
	uint8_t *t = (uint8_t *)addr;
	size_t		i = 0;
	struct iphdr iphdr = *addr;
	uint16_t flags;
	uint16_t fragment_offset;


	while (i < size) {
		dprintf(STDOUT_FILENO, "%s%02x", i % 2 == 0 ? " " : "", t[i]);
		i++;
	}
	dprintf(STDOUT_FILENO, "\n");

	dprintf(STDOUT_FILENO, "Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");

	// Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data
	// 4  5  00 0054 9bc8   2 0000  01  01 6b4c 172.21.196.125  1.1.1.1 

	dprintf(STDOUT_FILENO, " %1x", iphdr.version);

	dprintf(STDOUT_FILENO, "  %1x", iphdr.ihl);

	dprintf(STDOUT_FILENO, "  %02x", iphdr.tos);

	dprintf(STDOUT_FILENO, " ");
	printBytes(&iphdr.tot_len, sizeof(iphdr.tot_len));

	dprintf(STDOUT_FILENO, " ");
	printBytes(&iphdr.id, sizeof(iphdr.id));

	// Need ntohs for field > 8 bits
	flags = (ntohs(iphdr.frag_off) & 0xE000u) >> 13;
	dprintf(STDOUT_FILENO, "   %1x", flags); // 3 / 16 bits
	fragment_offset = ntohs(iphdr.frag_off) & 0x1FFF;
	dprintf(STDOUT_FILENO, " %04x", fragment_offset); // 13 / 16 bits

	dprintf(STDOUT_FILENO, "  %02x", iphdr.ttl);

	dprintf(STDOUT_FILENO, "  %02x", iphdr.protocol);

	dprintf(STDOUT_FILENO, " ");
	printBytes(&iphdr.check, sizeof(iphdr.check));

	char src[INET_ADDRSTRLEN];
	ft_memset(&src[0], 0, INET_ADDRSTRLEN);
	if (inet_ntop(AF_INET, &iphdr.saddr, &src[0], INET_ADDRSTRLEN) == NULL) {
		cleanPing();
		printError("ERROR: ntop");
		exit(FAILURE);
	}
	dprintf(STDOUT_FILENO, " %s ", src);

	char dst[INET_ADDRSTRLEN];
	ft_memset(&dst[0], 0, INET_ADDRSTRLEN);
	if (inet_ntop(AF_INET, &iphdr.daddr, &dst[0], INET_ADDRSTRLEN) == NULL) {
		cleanPing();
		printError("ERROR: ntop");
		exit(FAILURE);
	}
	dprintf(STDOUT_FILENO, " %s ", dst);

	dprintf(STDOUT_FILENO, "\n");
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
