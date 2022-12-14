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

# include "libft.h"

# define FAILURE -1
# define SUCCESS 0
# define ICMP_PACKET_LEN 8
# define DEBUG true


typedef struct s_socket t_socket;
struct s_socket {
  int family;
  int socktype;
  int protocol;
  int sockfd;
  t_socket *next;
};

typedef struct s_clock{
	unsigned char state;
	struct timeval tvf;
	struct timeval tvi;
	long diff_sec;
	long diff_usec;
	double diff;
} t_clock;

typedef struct s_ftping {
	t_clock time;
	uid_t uid;
	pid_t pid;
	unsigned int seq;
	char *node;
	char *service;

	char *canonname;
	struct addrinfo hints;
	struct addrinfo *results;
	struct sockaddr dest_addr;
	struct addrinfo *reverse;

	struct in_addr  addr_in;
	char rslv_node[INET_ADDRSTRLEN];
	char rslv_node2[INET_ADDRSTRLEN];
	char *ip_str;
	char *ip_str2;

	socklen_t addrlen;
	t_socket socket;
	// Packet buf ? + len
	int send_flags;
	struct icmp icmp;
	// struct iphdr ipheader;

	char control[100];
	char databuf[100];
	int rec_flags;
	struct msghdr msg;
	struct iovec iov[1];
	struct cmsghdr *cmhdr;
	struct sockaddr_in sin;
	unsigned char tos;
	struct cmsghdr *cmhdrptr;

} t_ftping;
// getpid getuid;
extern t_ftping *g_ping;

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
void printTSocket(t_socket *sckt);
void printAiInfo();

/* socket.c */
int getSocketFrom(t_socket *sckt, t_socket data);
int getSimpleSocket();
void getSockAddr(struct addrinfo *ptr, t_ftping *data);

/* packet.c */
void fillIcmp();
void setupInput();
void setupOutput();
void printMsg(int len);
void getAInfo();
void setInitialClock();
void setFinalClock();
void getTimeDiff();


/* signals.c */
void setHandler(int signum, sighandler_t handler);
void setAlarmHandler(int sec, sighandler_t handler);
void freePing();
void	loopHandler(int signum);
void	interruptHandler(int signal);
void	timeoutHandler(int signal);

/* recieve.c */
void recieveMsg();

/* tools.c */
void	print_memory(const void *addr, size_t size);
void printIcmp(struct icmp *icmptr);
void printIp(struct ip *ipptr);

/* looping.c */
void looping ();

#endif
