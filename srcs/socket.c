#include "ft_ping.h"

int setSocket(t_socket *sckt) {
  sckt->sockfd = socket(sckt->family, sckt->socktype, sckt->protocol);
  if (sckt->sockfd == -1) {
    return FAILURE;
  }
  return SUCCESS;
}

void setRawSocket() {
  int res;
  struct icmp_filter filter;
  t_socket *sock_ptr;
  int yes = 1;

  sock_ptr = &g_ping->socket;
  sock_ptr->family = AF_INET;
  sock_ptr->socktype = SOCK_RAW;
  sock_ptr->protocol = IPPROTO_ICMP;

  res = setSocket(sock_ptr);
  if (res == FAILURE) {
    printError("ERROR: %s | Failed getting a socket\n", strerror(errno));
    exit(FAILURE);
  }

  // ICMP_FILTER use a bitmask of the ICMP_CODE (ex: 1 << ICMP_ECHO)
  // ICMP_ECHOREPLY is code 0 so all bits are set except the lowest one
  filter.data = 0xFFFFFFFEu;
  res = setsockopt(sock_ptr->sockfd, SOL_RAW, ICMP_FILTER, &filter, sizeof(filter));
  if (res == -1) {
    printError("ERROR: %s | Failed setting a socket option\n", strerror(errno));
    exit(FAILURE);
  }

  // able to send to broadcast address
  res = setsockopt(sock_ptr->sockfd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
  if (res == -1) {
    printError("ERROR: %s | Failed setting a socket option\n", strerror(errno));
    exit(FAILURE);
  }
}
