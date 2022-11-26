#include "ft_ping.h"
#include "libft.h"
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

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
SOCK_STREAM     
};

const char* getFamilyName(int value) {
#define NAME(ERR) case ERR: return #ERR;
    switch (value) {
		NAME(AF_ALG       )
		NAME(AF_APPLETALK )
		NAME(AF_AX25      )
		NAME(AF_BLUETOOTH )
		NAME(AF_CAN       )
		NAME(AF_IB        )
		NAME(AF_INET      )
		NAME(AF_INET6     )
		NAME(AF_IPX       )
		NAME(AF_KCM       )
		NAME(AF_KEY       )
		NAME(AF_LLC       )
		// NAME(AF_LOCAL     ) local is 1 same as unix here
		NAME(AF_UNIX      )
		NAME(AF_MPLS      )
		NAME(AF_NETLINK   )
		NAME(AF_PACKET    )
		NAME(AF_PPPOX     )
		NAME(AF_RDS       )
		NAME(AF_TIPC      )
		NAME(AF_VSOCK     )
		NAME(AF_X25       )
		NAME(AF_XDP       )
	}
    return "unknown";
#undef NAME
}

const char* getSocketName(int value) {
#define NAME(ERR) case ERR: return #ERR;
    switch (value) {
		NAME(SOCK_CLOEXEC    )
		NAME(SOCK_DGRAM      )
		NAME(SOCK_NONBLOCK   )
		NAME(SOCK_PACKET     )
		NAME(SOCK_RAW        )
		NAME(SOCK_RDM        )
		NAME(SOCK_SEQPACKET  )
		NAME(SOCK_STREAM     )
    }
    return "unknown";
#undef NAME
}

// /etc/protocols
//
void printSocket(int family, int socktype, int protocol) {
	const char *fname;
	const char *sname;
	struct protoent *ptcl;
	char *pname = NULL;

	fname = getFamilyName(family);
	sname = getSocketName(socktype);
	ptcl = getprotobynumber(protocol);
	if (ptcl != NULL) {
		pname = ptcl->p_name;
	}
	printf("socket with:\n");
	printf("family |%s| %d \n", fname, family);
	printf("socket type |%s| %d \n", sname, socktype);
	printf("protocol |%s| %d \n", pname, protocol);
}
void printTSocket(t_socket *sckt) {
	printSocket(sckt->family, sckt->socktype, sckt->protocol);
	printf("sockfd |%d|\n", sckt->sockfd);
}

int getSocketFrom(t_socket *sckt, t_socket data)
{
	int family = data.family;
	int socktype = data.socktype;
	int protocol = data.protocol;
	printf("+++socket\n");
	sckt->family = family;
	sckt->socktype = socktype;
	sckt->protocol = protocol;

	printSocket(family, socktype, protocol);
	sckt->sockfd = socket(family, socktype, protocol);
	if (sckt->sockfd == -1) {
		printf("\n====\nERROR:%s\n====\n", strerror(errno));
		printf("Failed getting a socket\n");
		return FAILURE;
	}
	printTSocket(sckt);
	return SUCCESS;
}

int getSimpleSocket(t_ftping *data)
{
	int res;
	data->socket.family = AF_INET;
	data->socket.socktype = SOCK_STREAM;
	data->socket.protocol = 0;

	res = getSocketFrom(&(data->socket), data->socket);
	if (res == -1) {
		printf("GETSIMPLESOCKET\n");
	}
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
	t_socket sckt;

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


	res = getaddrinfo(pingdata.node, pingdata.service, &(pingdata.hints), &(pingdata.results));
	if (res < 0) {
		printf("%s\n", gai_strerror(res));
		return res;
	}
	rsltptr = pingdata.results;
	printf("list==\n");
	while (rsltptr != NULL)
	{
		printf("===AI BEGIN\n");
		printf("flags = %d\nfamily = %d\nsocktype = %d\nprotocol = %d\naddlen = %u\ncanon name = %s\nnext = %p\n\n",
				rsltptr->ai_flags, rsltptr->ai_family, rsltptr->ai_socktype, rsltptr->ai_protocol, rsltptr->ai_addrlen, rsltptr->ai_canonname, rsltptr->ai_next);

		sckt = (t_socket){
			.family = rsltptr->ai_family,
			.socktype = rsltptr->ai_socktype,
			.protocol = rsltptr->ai_protocol
		};
		if (sckt.socktype == SOCK_RAW)
		{
			sckt.protocol = IPPROTO_RAW;
		}

		res = getSocketFrom(&(pingdata.socket), sckt);
		if (res == -1) {
			freeaddrinfo(pingdata.results);
			exit(1);
		}
		// Do something with socket
		rsltptr = rsltptr->ai_next;
		printf("===AI END\n\n");
	}
	printf("list end===\n");
	freeaddrinfo(pingdata.results);
	pingdata.results = NULL;

	res = getSimpleSocket(&pingdata);
	if (res == -1) {
		printf("Failed getting a socket\n");
		return res;
	}
printf("OUT\n");
	return 0;
}
		// typedef void (*sighandler_t)(int);
		// sighandler_t signal(int signum, sighandler_t handler);
		// unsigned int alarm(unsigned int seconds);
		//

		// ft_memset(&packet, 0, sizeof(packet));
		// // fills packet
		// packet.type = 8;
		// packet.code = 0;
		// packet.checksum = 0;
		// packet.identifier = 0;
		// packet.sequence_number = 0;
		// packet.playload = 0;
		// if (res < 0) {
		// 	printf("FAIL set sock option |%d|%d|\n", res, errno);
		// 	exit(1);
		// }
		
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

