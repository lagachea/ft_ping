#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define FAILURE -1
#define SUCCESS 0

struct s_icmp {
  unsigned int type : 8;
  unsigned int code : 8;
  unsigned int checksum : 16;
  unsigned int identifier : 16;
  unsigned int sequence_number : 16;
  unsigned int playload : 32;
};
typedef struct s_icmp t_icmp;

typedef struct s_socket t_socket;
struct s_socket {
	int family;
	int socktype;
	int protocol;
	int sockfd;
	t_socket *next;
};

typedef struct s_ftping {
	uid_t uid;
	pid_t pid;
	char *node;
	char *service;
	struct addrinfo hints;
	struct addrinfo *results;
	int address;
	t_socket socket;
	t_socket *sockets;
	//Packet buf ? + len
	void *buf;
	struct sockaddr dest_addr;
	socklen_t addrlen;
	int send_flags;
	int rec_flags;
	struct msghdr msg;
} t_ftping;
// getpid getuid;
t_ftping *g_pingdata;

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
