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

  g_ping->socket.family = AF_INET;
  g_ping->socket.socktype = SOCK_RAW;
  g_ping->socket.protocol = IPPROTO_ICMP;

  res = getSocket(&g_ping->socket);
  if (res == FAILURE) {
    exit(FAILURE);
  }


  return 0;
}
