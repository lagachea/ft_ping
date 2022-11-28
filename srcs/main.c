#include "ft_ping.h"
#include "libft.h"
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

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
	if (sckt->socktype == SOCK_RAW)
	{
		if (sckt->protocol == IPPROTO_IP) {
			printf("switch from IP TO ICMP for %s\n", getSocketName(SOCK_RAW));
			sckt->protocol = IPPROTO_ICMP;
		}
		if (g_pingdata->uid != 0) {
			printf("no raw socket without sudo switching to DGRAM\n");
			sckt->socktype = SOCK_DGRAM;
		}
		else
		{
			printf("shall we try as root to get a %s\n", getSocketName(SOCK_RAW));
		}
	}

	printf("ask for ");
	printSocket(sckt->family, sckt->socktype, sckt->protocol);
	sckt->sockfd = socket(sckt->family, sckt->socktype, sckt->protocol);
	if (sckt->sockfd == -1) {
		printf("\n====\nERROR:%s\n====\n", strerror(errno));
		printf("Failed getting a socket\n");
		return FAILURE;
	}
	printf("got ");
	printTSocket(sckt);
	printf("---socket\n");
	return SUCCESS;
}

int getSimpleSocket()
{
	int res;
	g_pingdata->socket.family = AF_INET;
	g_pingdata->socket.socktype = SOCK_RAW;
	g_pingdata->socket.protocol = IPPROTO_ICMP;

	printSocket(g_pingdata->socket.family, g_pingdata->socket.socktype, g_pingdata->socket.protocol);
	res = getSocketFrom(&(g_pingdata->socket), g_pingdata->socket);
	if (res == -1) {
		printf("Failed getting a socket\n");
		exit(1);
	}
	return 0;
}

void getSockAddr(struct addrinfo *ptr, t_ftping *data) {
	struct sockaddr_in *sock_addr = (struct sockaddr_in *)ptr->ai_addr;
	char addrstr[INET_ADDRSTRLEN];
	char *addr;


	addr = &(addrstr[0]);
	(void)data;
	inet_ntop(ptr->ai_family, &sock_addr->sin_addr, addr, INET_ADDRSTRLEN);
	inet_pton(ptr->ai_family, addr, &data->address);

	printf("addr => %d\n", data->address);
	printf("address %s\t", addr);		
	printf("canon name = %s\t",ptr->ai_canonname);		
	data->dest_addr = *(ptr->ai_addr);
	printf("\ndest_addr set to address\n");		
}

int	main(void)
{
printf("IN\n");

	t_ftping pingdata;
	g_pingdata = &pingdata;
	int res;
	char node[40] = "192.168.1.1";
	// "9.9.9.9;www.42.fr;ms-17;google.com";
	char service[] = "";
	struct addrinfo *rsltptr;
	t_socket sckt;

	ft_memset(&pingdata, 0, sizeof(pingdata));
	g_pingdata->node = &(node[0]);
	g_pingdata->service = &(service[0]);
	g_pingdata->service = NULL;

	g_pingdata->uid = getuid();
	g_pingdata->pid = getpid();
	printf("PID=%10d | UID=%10d\n", g_pingdata->pid, g_pingdata->uid);

	printf("node => %s\n", g_pingdata->node);
	printf("service => %s\n", g_pingdata->service);
	printf("=====\n");


	g_pingdata->hints.ai_family = AF_INET;
	g_pingdata->hints.ai_socktype = 0;
	g_pingdata->hints.ai_protocol = 0;
	g_pingdata->hints.ai_flags = 0;
	g_pingdata->hints.ai_canonname = NULL;
	g_pingdata->hints.ai_addr = NULL;
	g_pingdata->hints.ai_next = NULL;

	getSimpleSocket();
	close(g_pingdata->socket.sockfd);
	printf("closed sockfd %d\n",g_pingdata->socket.sockfd);

	res = getaddrinfo(g_pingdata->node, g_pingdata->service, &(g_pingdata->hints), &(g_pingdata->results));
	if (res < 0) {
		printf("%s\n", gai_strerror(res));
		return res;
	}
	rsltptr = g_pingdata->results;
	printf("list==\n");
	while (rsltptr != NULL)
	{
		printf("===AI BEGIN\n");
		getSockAddr(rsltptr, g_pingdata);

		sckt = (t_socket){
			.family = rsltptr->ai_family,
			.socktype = rsltptr->ai_socktype,
			.protocol = rsltptr->ai_protocol
		};
		// else {
			res = getSocketFrom(&(g_pingdata->socket), sckt);
			if (res == -1) {
				freeaddrinfo(g_pingdata->results);
				exit(1);
			}
			// Do something with socket
			close(sckt.sockfd);
			printf("close sockfd %d\n", sckt.sockfd);
		// }
		rsltptr = rsltptr->ai_next;
		printf("===AI END\n\n");
	}
	printf("list end===\n");
	freeaddrinfo(g_pingdata->results);
	g_pingdata->results = NULL;

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

