#include "ft_ping.h"
#include "libft.h"

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

// /etc/protocols

int printSocket(t_ftping *data){
	printf("got sockfd %d \n", data->socket.sockfd);
	return 0;
};

int getSocket(t_ftping *data){
	data->socket.protocol = 0;
	data->socket.socktype = SOCK_STREAM;
	data->socket.family = AF_INET;

	printf("asked for socket with IPV4 = %d\nSOCK_STREAM=%d\nprotocole=%d\n", data->socket.family, data->socket.socktype, data->socket.protocol);
	data->socket.sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printSocket(data);
	return 0;
}

int getAddrinfo(t_ftping *data) {
	int res;
	res = getaddrinfo(data->node, data->service, &(data->hints), &(data->results));
	return res;
}

int	main(void)
{
printf("IN\n");

	t_ftping pingdata;
	int res;
	unsigned int in_addr = 0;
	char address[40] = "9.9.9.9";
	// "192.168.1.1";
	// "google.com";
	char socktype[] = "";
	struct addrinfo *rsltptr;

	t_icmp packet;

	ft_memset(&pingdata, 0, sizeof(pingdata));

	pingdata.uid = getuid();
	pingdata.pid = getpid();
	printf("PID=%10d | UID=%10d\n", pingdata.pid, pingdata.uid);

	inet_pton(AF_INET, address, &in_addr);
	inet_ntop(AF_INET, &in_addr, address, sizeof(address));
	printf("in_addr => %u\n",in_addr);
	printf("address => %s\n",address);
	printf("socktype => %s\n",socktype);
	printf("=====\n");

	pingdata.node = &(address[0]);
	pingdata.service = &(socktype[0]);

	pingdata.hints.ai_flags = AI_PASSIVE ^ AI_CANONNAME;
	pingdata.hints.ai_family = AF_INET;
	pingdata.hints.ai_socktype = 0;
	pingdata.hints.ai_protocol = 0;
	pingdata.hints.ai_canonname = NULL;
	pingdata.hints.ai_addr = NULL;
	pingdata.hints.ai_next = NULL;


	res = getAddrinfo(&pingdata);
	if (res < 0) {
		printf("%s\n", gai_strerror(res));
		return res;
	}
	rsltptr = pingdata.results;
	printf("===\nlist==\n");
	while (rsltptr != NULL)
	{
		printf("===AI\n");
		printf("flags = %d\nfamily = %d\nsocktype = %d\nprotocol = %d\naddlen = %u\ncanon name = %s\nnext = %p\n",
				rsltptr->ai_flags, rsltptr->ai_family, rsltptr->ai_socktype, rsltptr->ai_protocol, rsltptr->ai_addrlen, rsltptr->ai_canonname, rsltptr->ai_next);

		// typedef void (*sighandler_t)(int);
		// sighandler_t signal(int signum, sighandler_t handler);
		// unsigned int alarm(unsigned int seconds);
		//
		rsltptr = rsltptr->ai_next;
		printf("\n\n");
	}
	freeaddrinfo(pingdata.results);
	pingdata.results = NULL;
	printf("=====\n");

	getSocket(&pingdata);
	if (pingdata.socket.sockfd != -1) {
		printf("===socket\n");
		// printSocket(rsltptr, sockfd);

		ft_memset(&packet, 0, sizeof(packet));
		// fills packet
		packet.type = 8;
		packet.code = 0;
		packet.checksum = 0;
		packet.identifier = 0;
		packet.sequence_number = 0;
		packet.playload = 0;
		if (res < 0) {
			printf("FAIL set sock option |%d|%d|\n", res, errno);
			exit(1);
		}
		
		// res = sendto(pingdata.socket.sockfd, pingdata.buf, 0, pingdata.send_flags, &(pingdata.dest_addr), pingdata.addrlen);
		// if (res < 0) {
		// 	printf("FAIL send to |%d|%d|\n", res, errno);
		// 	exit(1);
		// }
		//
		// res = recvmsg(pingdata.socket.sockfd, &(pingdata.msg), pingdata.rec_flags);
		// if (res < 0) {
		// 	printf("FAIL RECV |%d|%d|\n", res, errno);
		// 	exit(1);
		// }
	}

	return res;
printf("OUT\n");
	return 0;
}
