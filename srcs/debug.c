#include "ft_ping.h"

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
		NAME(AF_ALG       );
		NAME(AF_APPLETALK );
		NAME(AF_AX25      );
		NAME(AF_BLUETOOTH );
		NAME(AF_CAN       );
		NAME(AF_IB        );
		NAME(AF_INET      );
		NAME(AF_INET6     );
		NAME(AF_IPX       );
		NAME(AF_KCM       );
		NAME(AF_KEY       );
		NAME(AF_LLC       );
		NAME(AF_UNIX      );
		NAME(AF_MPLS      );
		NAME(AF_NETLINK   );
		NAME(AF_PACKET    );
		NAME(AF_PPPOX     );
		NAME(AF_RDS       );
		NAME(AF_TIPC      );
		NAME(AF_VSOCK     );
		NAME(AF_X25       );
		NAME(AF_XDP       );
	}
    return "unknown";
#undef NAME
}

const char* getSocketName(int value) {
#define NAME(ERR) case ERR: return #ERR;
    switch (value) {
		NAME(SOCK_CLOEXEC    );
		NAME(SOCK_DGRAM      );
		NAME(SOCK_NONBLOCK   );
		NAME(SOCK_PACKET     );
		NAME(SOCK_RAW        );
		NAME(SOCK_RDM        );
		NAME(SOCK_SEQPACKET  );
		NAME(SOCK_STREAM     );
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
	struct sockaddr addr;
	socklen_t len;
	int res;

	len = sizeof(addr);
	res = getsockname(sckt->sockfd, &addr, &len);
	if (res == -1) {
		printf("\n====\nERROR:%s\n====\n", strerror(errno));
	}
	printf("GOT SOCK %s |%s|\n",getFamilyName(addr.sa_family), addr.sa_data);
	printSocket(sckt->family, sckt->socktype, sckt->protocol);
	printf("sockfd |%d|\n", sckt->sockfd);
}

void printAiInfo() {
	struct addrinfo *ai_res;

	ai_res = g_ping->results;
	printf("%s\n", ai_res->ai_canonname);
	while (ai_res) {
		printf("\n");
		print_memory(ai_res, sizeof(struct addrinfo));
		printf("\n");
		ai_res = (ai_res)->ai_next;
	}
}

void printArg(int ac, char **av) {
	int iter = 0;
	while (++iter < ac)
	{
	  printf("%s", av[iter]);
	  if (iter > 1)
		  printf(" | ");
	  if (iter + 1 == ac)
		  printf("\n");
	}
}
