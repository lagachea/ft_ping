#include "ft_ping.h"

int getSocket(t_socket *sckt) {
  sckt->sockfd = socket(sckt->family, sckt->socktype, sckt->protocol);
  if (sckt->sockfd == -1) {
    return FAILURE;
  }
  return SUCCESS;
}

int getRawSocket() {
  int res;
  struct icmp_filter filter;
  int yes = 1;

  g_ping->socket.family = AF_INET;
  g_ping->socket.socktype = SOCK_RAW;
  g_ping->socket.protocol = IPPROTO_ICMP;

  res = getSocket(&g_ping->socket);
  if (res == FAILURE) {
    printError("ERROR: %s | Failed getting a socket\n", strerror(errno));
    exit(FAILURE);
  }

  // ICMP_FILTER use a bitmask of the ICMP_CODE (ex: 1 << ICMP_ECHO)
  // ICMP_ECHOREPLY is code 0 so all bits are set except the lowest one
  filter.data = 0xFFFFFFFEu;
  res = setsockopt(g_ping->socket.sockfd, SOL_RAW, ICMP_FILTER, &filter, sizeof(filter));
  if (res == -1) {
    printError("ERROR: %s | Failed setting a socket option\n", strerror(errno));
    exit(FAILURE);
  }

  // able to send to broadcast address
  res = setsockopt(g_ping->socket.sockfd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
  if (res == -1) {
    printError("ERROR: %s | Failed setting a socket option\n", strerror(errno));
    exit(FAILURE);
  }

  return 0;
}
