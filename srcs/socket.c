#include "ft_ping.h"

int getSocketFrom(t_socket *sckt, t_socket data) {

  int family = data.family;
  int socktype = data.socktype;
  int protocol = data.protocol;
  // printf("+++socket\n");
  sckt->family = family;
  sckt->socktype = socktype;
  sckt->protocol = protocol;
  if (sckt->socktype == SOCK_RAW) {
    if (sckt->protocol == IPPROTO_IP) {
      // printf("switch from IP TO ICMP for %s\n", getSocketName(SOCK_RAW));
      sckt->protocol = IPPROTO_ICMP;
    }
    if (g_ping->uid != 0) {
      // printf("no raw socket without sudo switching to DGRAM\n");
      sckt->socktype = SOCK_DGRAM;
    } else {
      // printf("shall we try as root to get a %s\n", getSocketName(SOCK_RAW));
    }
  }

  // printf("ask for ");
  // printSocket(sckt->family, sckt->socktype, sckt->protocol);
  sckt->sockfd = socket(sckt->family, sckt->socktype, sckt->protocol);
  if (sckt->sockfd == -1) {
    printf("\n====\nERROR:%s\n====\n", strerror(errno));
    printf("Failed getting a socket\n");
    return FAILURE;
  }
  // printf("got ");
  // printTSocket(sckt);
  // printf("---socket\n");
  return SUCCESS;
}

int getSimpleSocket() {
  int res;
  int hdrincl;
  int so_debug;

  hdrincl = 0;
  so_debug = 1;
  g_ping->socket.family = AF_INET;
  g_ping->socket.socktype = SOCK_RAW;
  g_ping->socket.protocol = IPPROTO_ICMP;

  res = getSocketFrom(&(g_ping->socket), g_ping->socket);
  if (res == -1) {
    printf("Failed getting a socket\n");
    exit(1);
  }
  setsockopt(g_ping->socket.sockfd, SOL_SOCKET, IP_HDRINCL, &hdrincl,
             sizeof(hdrincl));
  if (res == -1) {
    printf("Failed setting a socket option\n");
    exit(1);
  }
  setsockopt(g_ping->socket.sockfd, SOL_SOCKET, SO_DEBUG, &so_debug,
             sizeof(so_debug));
  if (res == -1) {
    printf("Failed setting a socket option\n");
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
  inet_pton(ptr->ai_family, addr, &data->addr_in);

  // printf("addr => %d\n", data->addr_in);
  printf("address %s\t", addr);
  printf("canon name = %s\t", ptr->ai_canonname);
  data->dest_addr = *(ptr->ai_addr);
  printf("\ndest_addr set to address\n");
}
