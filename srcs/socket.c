#include "ft_ping.h"

int getSocket(t_socket *sckt) {
  sckt->sockfd = socket(sckt->family, sckt->socktype, sckt->protocol);
  if (sckt->sockfd == -1) {
    printError("ERROR:%s | Failed getting a socket\n", strerror(errno));
    return FAILURE;
  }
  return SUCCESS;
}

int getRawSocket() {
  int res;
  // int off;
  // int on;

  // off = 0;
  // on = 1;
  g_ping->socket.family = AF_INET;
  g_ping->socket.socktype = SOCK_RAW;
  g_ping->socket.protocol = IPPROTO_ICMP;

  res = getSocket(&g_ping->socket);
  if (res == FAILURE) {
    exit(FAILURE);
  }

 //  res = setsockopt(g_ping->socket.sockfd, IPPROTO_IP, IP_HDRINCL, &off, sizeof(off));
 //  if (res == -1) {
	// printError("ERROR:%s | Failed setting socket option 1\n", strerror(errno));
 //    exit(FAILURE);
 //  }

 //  res = setsockopt(g_ping->socket.sockfd, SOL_SOCKET, SO_DEBUG, &on, sizeof(on));
 //  if (res == -1) {
	// printError("ERROR:%s | Failed setting socket option 2\n", strerror(errno));
 //    exit(FAILURE);
 //  }

  return 0;
}

void getSockAddr(struct addrinfo *ptr, t_ftping *data) {
  struct sockaddr_in *sock_addr = (struct sockaddr_in *)ptr->ai_addr;
  char addrstr[INET_ADDRSTRLEN];
  char *addr;

  addr = &(addrstr[0]);
  inet_ntop(ptr->ai_family, &sock_addr->sin_addr, addr, INET_ADDRSTRLEN);
  inet_pton(ptr->ai_family, addr, &data->addr_in);

  // printf("addr => %d\n", data->addr_in);
  printf("address %s\t", addr);
  printf("canon name = %s\t", ptr->ai_canonname);
  data->dest_addr = *(ptr->ai_addr);
  printf("\ndest_addr set to address\n");
}
