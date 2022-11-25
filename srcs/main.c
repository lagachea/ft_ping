#include "ft_ping.h"
#include "libft.h"
       #include <sys/socket.h>
       #include <netdb.h>

uid_t getuid(void);
pid_t getpid(void);

int getaddrinfo(const char *node, const char *service,
		const struct addrinfo *hints,
		struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);
const char *gai_strerror(int errcode);

const char *inet_ntop(int af, const void *src,
		char *dst, socklen_t size);
int inet_pton(int af, const char *src, void *dst);

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
unsigned int alarm(unsigned int seconds);

int socket(int domain, int type, int protocol);
int setsockopt(int sockfd, int level, int optname,
		const void *optval, socklen_t optlen);

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
		const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
int printf(const char *format, ...);

// /etc/protocols
int sockets[24] = {
SOCK_CLOEXEC    ,
SOCK_DGRAM      ,
SOCK_NONBLOCK   ,
SOCK_PACKET     ,
SOCK_RAW        ,
SOCK_RDM        ,
SOCK_SEQPACKET  ,
SOCK_STREAM     ,
};

int families[24] = {
AF_ALG       ,
AF_APPLETALK ,
AF_AX25      ,
AF_BLUETOOTH ,
AF_CAN       ,
AF_IB        ,
AF_INET      ,
AF_INET6     ,
AF_IPX       ,
AF_KCM       ,
AF_KEY       ,
AF_LLC       ,
AF_LOCAL     ,
AF_MPLS      ,
AF_NETLINK   ,
AF_PACKET    ,
AF_PPPOX     ,
AF_RDS       ,
AF_TIPC      ,
AF_UNIX      ,
AF_VSOCK     ,
AF_X25       ,
AF_XDP       
};


int printSocket(struct addrinfo *rsltptr, int sockfd){
	for (size_t iter = 0; iter < 24; iter++)
	{
		if (rsltptr->ai_family == families[iter]) {
			printf("family %d\n", families[iter]);
		}
		if (rsltptr->ai_socktype == sockets[iter]) {
			printf("socket %d\n", sockets[iter]);
		}
	}
	printf("protocol %d\n", rsltptr->ai_protocol);
	printf("sockfd %d \n", sockfd);
	return 0;
};

int	main(void)
{
printf("IN\n");

	uid_t uid;
	pid_t pid;
	int res;
	char address[40] = "192.168.1.1";
	// "8.8.8.8";
	// "google.com";
	unsigned int in_addr = 0;
	char socktype[] = "";
	struct addrinfo hints;
	struct addrinfo *result;
	struct addrinfo *rsltptr;

	int sockfd = 0;
	void *buf = NULL;
	size_t buflen = 0;
	int send_flags = 0;

	const struct sockaddr dest_addr;
	struct msghdr msg;
	int rec_flags = 0;

	ft_memset(&hints, 0, sizeof(hints));
	ft_memset(&result, 0, sizeof(&result));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	uid = getuid();
	pid = getpid();
	printf("PID=%10d | UID=%10d\n", pid, uid);

	inet_pton(AF_INET, address, &in_addr);
	inet_ntop(AF_INET, &in_addr, address, sizeof(address));
	printf("address => %s\n",address);
	printf("in_addr => %u\n",in_addr);
	printf("socktype => %s\n",socktype);
	printf("=====\n");

	res = getaddrinfo(address, socktype, &hints, &result);
	if (res < 0) {
		printf("%s\n", gai_strerror(res));
		return res;
	}

	rsltptr = result;
	printf("===\nlist==\n");
	while (rsltptr != NULL)
	{
		printf("===AI\n");
		printf("flags = %d\nfamily = %d\nsocktype = %d\nprotocol = %d\naddlen = %u\ncanon name = %s\nnext = %p\n",
				rsltptr->ai_flags, rsltptr->ai_family, rsltptr->ai_socktype, rsltptr->ai_protocol, rsltptr->ai_addrlen, rsltptr->ai_canonname, rsltptr->ai_next);

		// typedef void (*sighandler_t)(int);
		// sighandler_t signal(int signum, sighandler_t handler);
		// unsigned int alarm(unsigned int seconds);
		sockfd = socket(rsltptr->ai_family, rsltptr->ai_socktype, rsltptr->ai_protocol);
		if (sockfd != -1) {
			printf("===socket\n");
			printSocket(rsltptr, sockfd);
			// int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
			res = sendto(sockfd, buf, buflen, send_flags, &dest_addr, rsltptr->ai_addrlen);
			if (res != -1) {
				printf("BB\n");
				res = recvmsg(sockfd, &msg, rec_flags);
				if (res != -1) {
					printf("AA\n");
				}
			}
		}

		rsltptr = rsltptr->ai_next;
		printf("\n\n");
	}
	freeaddrinfo(result);
	result = NULL;
	printf("=====\n");

	return res;
// prepare packet
// send packet

printf("OUT\n");
	return 0;
}

