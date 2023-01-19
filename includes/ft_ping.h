#ifndef FT_PING_H
# define FT_PING_H

# include <arpa/inet.h>
# include <asm-generic/socket.h>
# include <bits/types/struct_timeval.h>
# include <errno.h>
# include <netdb.h>
# include <netinet/in.h>
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
# define NEXT 1
# define IP 1
# define HOSTNAME -1
# define PACKET_LEN 20 + ICMP_MINLEN

/* OPTIONS */
# define VERBOSE_OPTION 1

union networkAddress {
	uint32_t integer;
	unsigned char bytes[4];
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
	struct timeval tvo;
	struct timeval tvf;
	struct timeval tvi;
	long int diff;
	double diff_ms;
} t_clock;

typedef struct s_ftping {
	char options;
	char state;
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

// getpid getuid;
uid_t getuid(void);
pid_t getpid(void);

// getaddrinfo freeaddrinfo gai_strerror;

int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints, struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);
const char *gai_strerror(int errcode);

// struct addrinfo {
// 	int              ai_flags;
// 	int              ai_family;
// 	int              ai_socktype;
// 	int              ai_protocol;
// 	socklen_t        ai_addrlen;
// 	struct sockaddr *ai_addr;
// 	char            *ai_canonname;
// 	struct addrinfo *ai_next;
// };

// gettimeofday;

// int gettimeofday(struct timeval *tv, struct timezone *tz);

// inet_ntop inet_pton;

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);

int inet_pton(int af, const char *src, void *dst);

// exit;

void exit(int status);

// alarm signal;

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
unsigned int alarm(unsigned int seconds);

// socket setsockopt;

int socket(int domain, int type, int protocol);
int setsockopt(int sockfd, int level, int optname, const void *optval,
               socklen_t optlen);
// int getsockopt(int sockfd, int level, int optname,
// 		void *optval, socklen_t *optlen);

// sendto recvmsg;

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);

// ssize_t send(int sockfd, const void *buf, size_t len, int flags);
// ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);

// ssize_t recv(int sockfd, void *buf, size_t len, int flags);
// ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
// 		struct sockaddr *src_addr, socklen_t *addrlen);

// xprintf;

int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int dprintf(int fd, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
// int vprintf(const char *format, va_list ap);
// int vfprintf(FILE *stream, const char *format, va_list ap);
// int vdprintf(int fd, const char *format, va_list ap);
// int vsprintf(char *str, const char *format, va_list ap);
// int vsnprintf(char *str, size_t size, const char *format, va_list ap);
//

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
void setInitialClock();
void setFinalClock();
void getTimeDiff();
unsigned int getDiff(struct timeval *tvf, struct timeval *tvi);
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
