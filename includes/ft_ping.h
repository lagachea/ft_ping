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


# include "libft.h"

# define FAILURE 1
# define SUCCESS 0
# define TIMEOUT 10
# define IP 1
# define HOSTNAME 2
# define PACKET_LEN 20 + ICMP_MINLEN
# define READY 0
# define SEND 1
# define RECIEVE 2
# define WAIT 3
# define ONE_SEC 1000000

/* OPTIONS */
# define VERBOSE_OPTION 1

union networkAddress {
	uint32_t integer;
	uint8_t bytes[4];
};

typedef struct s_socket t_socket;
struct s_socket {
  int family;
  int socktype;
  int protocol;
  int sockfd;
};

typedef struct s_stats{
	unsigned int transmitted;
	unsigned int recieved;
	unsigned int diff;
	unsigned int loss_percent;
	unsigned int sumtime;
	double min;
	double max;
	double avg;
	double mdev;
	double sum;
	double sum2;
} t_stats;

typedef struct s_clock{
	struct timeval original;
	struct timeval reception;
	struct timeval emission;
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

	char rslv_node[INET_ADDRSTRLEN];
	char *ip_str;

	t_socket socket;
	// Packet buf ? + len
	struct icmp icmp;
	// struct iphdr ipheader;

	char control[50];
	char databuf[50];
	int rec_flags;
	struct msghdr msg;
	struct iovec iov[1];
	struct sockaddr_in sin;

} t_ftping;
extern t_ftping *g_ping;

typedef void (*sighandler_t)(int);

/* debug.c */
const char *getFamilyName(int value);
const char *getSocketName(int value);
void printSocket(int family, int socktype, int protocol);
void printSocketStruct(t_socket *sckt);
void printAddressInformations();
void printArguments(int ac, char **av);
void printUsage();

/* socket.c */
int getSocket(t_socket *sckt);
int getRawSocket();
void getSockAddr(struct addrinfo *ptr, t_ftping *data);

/* packet.c */
void fillIcmp();
void setupInput();
void setupOutput();
void printMessageStatistics(int len);
void getAddressInformation();
void setClock(struct timeval *tv);
void setInitialTimestamp();
void setFinalClock();
void updateWaitClock();
void getRoudTripTime();
unsigned int getTimeDiff(struct timeval *tvf, struct timeval *tvi);
void setOriginalClock();


/* signals.c */
void setHandler(int signum, sighandler_t handler);
void setAlarmHandler(int sec, sighandler_t handler);
void freePing();
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

/* tools.c */
void	print_memory(const void *addr, size_t size);
void printIcmp(struct icmp *icmptr);
void printIp(struct ip *ipptr);
void printStatistics();
void printShortStatistics();
void printTimeval(struct timeval *tv);
int ft_strcountchr(char *str, int c);

/* looping.c */
void looping ();

/* arguments.c */
void parseArguments(int ac, char **av);
int parsedDestination();

/* error.c */
int printError(const char *fmt, ...);
#endif
