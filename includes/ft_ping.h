#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


//getpid;


pid_t getpid(void);

// pid_t getppid(void);


//getuid;


uid_t getuid(void);
uid_t geteuid(void);


//getaddrinfo;


int getaddrinfo(const char *node, const char *service,
		const struct addrinfo *hints,
		struct addrinfo **res);

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

// void freeaddrinfo(struct addrinfo *res);
// const char *gai_strerror(int errcode);


//gettimeofday;


// int gettimeofday(struct timeval *tv, struct timezone *tz);

// int settimeofday(const struct timeval *tv, const struct timezone *tz);


//inet_ntop;


const char *inet_ntop(int af, const void *src,
		char *dst, socklen_t size);


//inet_pton;


int inet_pton(int af, const char *src, void *dst);


//exit;


void exit(int status);


//signal;


typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);


//alarm;


unsigned int alarm(unsigned int seconds);

//setsockopt;

int setsockopt(int sockfd, int level, int optname,
		const void *optval, socklen_t optlen);

// int getsockopt(int sockfd, int level, int optname,
// 		void *optval, socklen_t *optlen);


//recvmsg;


ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);

// ssize_t recv(int sockfd, void *buf, size_t len, int flags);
// ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
// 		struct sockaddr *src_addr, socklen_t *addrlen);


//sendto;


ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		const struct sockaddr *dest_addr, socklen_t addrlen);

// ssize_t send(int sockfd, const void *buf, size_t len, int flags);
// ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);


//socket;


int socket(int domain, int type, int protocol);


//xprintf;


int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int dprintf(int fd, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int vprintf(const char *format, va_list ap);
int vfprintf(FILE *stream, const char *format, va_list ap);
int vdprintf(int fd, const char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
