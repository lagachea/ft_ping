#include "ft_ping.h"

static void icmpChecksum() {
	uint16_t res;
	uint16_t *ptr;
	size_t count;

	res = 0;
	ptr = (uint16_t *)&g_ping->icmp;
	count = 0;
	while (count < 8) {
		res += *ptr;
		ptr++;
		count++;
	}
	g_ping->icmp.icmp_cksum = ~res;
}

void fillIcmp() {
	g_ping->icmp.icmp_type = ICMP_ECHO;
	g_ping->icmp.icmp_code = 0;
	g_ping->icmp.icmp_hun.ih_idseq.icd_id = g_ping->pid;
	g_ping->icmp.icmp_hun.ih_idseq.icd_seq = g_ping->seq;
	icmpChecksum();
	g_ping->seq++;
}
