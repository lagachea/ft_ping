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
  // int ttl = TTL;

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
  filter.data = 0xFFFFFFFFu;
  filter.data -= 1 << ICMP_ECHOREPLY;
  filter.data -= 1 << ICMP_DEST_UNREACH;
  filter.data -= 1 << ICMP_SOURCE_QUENCH;
  filter.data -= 1 << ICMP_REDIRECT;
  filter.data -= 1 << ICMP_TIME_EXCEEDED;
  filter.data -= 1 << ICMP_PARAMETERPROB;

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
  
  // Set ttl
  // res = setsockopt(sock_ptr->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
  // if (res == -1) {
  //   printError("ERROR: %s | Failed setting a socket option\n", strerror(errno));
  //   exit(FAILURE);
  // }
}
