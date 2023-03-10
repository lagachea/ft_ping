#ifndef FT_PING_H
# define FT_PING_H

# include <arpa/inet.h>
# include <asm-generic/socket.h>
# include <bits/types/struct_timeval.h>
# include <errno.h>
# include <netdb.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <signal.h>
# include <stdarg.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>
# include <stdint.h>
# include <math.h>
# include <float.h>

/* THIS IS FROM <linux/icmp.h> but we cannot use both this and <netinet/ip_icmp.h>
*/
#define ICMP_FILTER			1

struct icmp_filter {
	uint32_t data;
};
/* end
*/

# include "libft.h"

// # define DEBUG 1
# define FAILURE 1
# define SUCCESS 0
# define USAGE 64
# define TIMEOUT 10
# define IP 1
# define HOSTNAME 2
# define LOCAL 4
# define ICMP_MINLEN 8
# define ICMP_DATA_LEN 56
# define ICMPHDR_LEN sizeof(struct icmphdr)
# define PACKET_LEN sizeof(struct iphdr) + ICMP_MINLEN
# define ICMP_FULL sizeof(t_icmp_out)
# define PACKET_FULL  sizeof(t_msg_packet)
# define READY 0
# define SEND 1
# define RECIEVE 2
# define WAIT 3
# define ONE_SEC 1000000
# define MSG_LEN 100

/* OPTIONS */
# define VERBOSE_OPTION 1

typedef struct s_icmp_out {
	struct icmphdr icmphdr;
	uint8_t data[ICMP_DATA_LEN];
}
t_icmp_out;

typedef struct s_msg_packet {
	struct iphdr iphdr;
	t_icmp_out icmp;
} t_msg_packet;

union networkAddress {
	uint32_t integer;
	uint8_t bytes[sizeof(struct in_addr)];
};

typedef struct s_socket t_socket;
struct s_socket {
  int family;
  int socktype;
  int protocol;
  int sockfd;
};

typedef struct s_stats{
	long int transmitted;
	long int recieved;
	long int diff;
	long int loss_percent;
	double min;
	double max;
	double avg;
	double mdev;
	double sum;
	double sum2;
} t_stats;

typedef struct s_clock{
	struct timeval emission;
	struct timeval msg;
	struct timeval reception;
	struct timeval wait;
	long int diff;
	double diff_ms;
} t_clock;

typedef struct s_step {
	unsigned char count: 2;
} t_step;

typedef struct s_ftping {
	unsigned char options;
	unsigned char state;
	t_step	step;
	t_clock time;
	t_stats counters;
	pid_t pid;
	unsigned short int seq;
	char *node;
	char *service;

	char hostname[NI_MAXHOST];
	char *canonname;
	struct addrinfo hints;
	struct addrinfo *results;
	struct sockaddr dest_addr;
	union networkAddress destination;

	char rslv_node[INET_ADDRSTRLEN];
	char *ip_str;

	t_socket socket;
	// Packet buf ? + len
	// struct iphdr ipheader;
	// struct icmphdr icmp;

	struct msghdr msg;
	struct iovec iov[1];
	char control[MSG_LEN];
	char databuf[MSG_LEN];
	int rec_flags;
	int msg_ret;

	t_msg_packet pkt_msg;
} t_ftping;
extern t_ftping *g_ping;

typedef void (*sighandler_t)(int);

/* debug.c */
const char *getFamilyName(int value);
const char *getSocketName(int value);
void printSocket(int family, int socktype, int protocol);
void printSocketStruct(t_socket *sckt);
void printAddressInformations();
void reverseDNSquery();
void printArguments(int ac, char **av);

/* socket.c */
void setRawSocket();
int setSocket(t_socket *sckt);

/* packet.c */
void printInitialInformation();
void setupRoundTrip();
void printMessageStatistics();
void setClock(struct timeval *tv);
void setInitialTimestamp();
void setReceptionClock();
void setWaitClock();
void setRoudTripTime();
long int getTimeDiff(struct timeval *tvf, struct timeval *tvi);
void setOriginalClock();
void sendPing();


/* signals.c */
void setHandler(int signum, sighandler_t handler);
void setTimeoutAlarm();
void resetAlarm();
void cleanPing();
void	loopHandler(int signum);
void	interruptHandler(int signal);
void	timeoutHandler(int signal);
void	sigquitHandler(int signal);

/* stats.c */
void setTransmitted();
void setRecieved();
void setLoss();
void updateStatistics();

/* recieve.c */
void recieveMessage();
void setupReception();
int isValidMessage();
void setMsgPointer();
int hasValidMessage();

/* tools.c */
void	printMemory(const void *addr, size_t size);
void printIcmp(struct icmphdr *icmptr);
void printIp(struct iphdr *ipptr);
void printStatistics();
void printShortStatistics();
void printTimeval(struct timeval *tv);
int ft_strcountchr(char *str, int c);
void printUsage();
int expectMessage();
void setup();

/* looping.c */
void pingRoundTrip ();

/* arguments.c */
void parseArguments(int ac, char **av);
int parsedDestination();

/* error.c */
int printError(const char *fmt, ...);
#endif
