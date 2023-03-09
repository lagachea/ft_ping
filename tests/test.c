#include "test.h"
#include "ft_ping.h"
#include <stdio.h>

#ifdef DEBUG
int __lsan_is_turned_off() { return 1; }
#endif

t_ftping *g_ping;
int tests_run = 0;

static char *test_sockets() { mu_assert("erro socket pas bon", 1 == 0); }

static char *test_socket() {
  // prequisite
  // a = b
  setRawSocket();

  // assert
  // a == b
  mu_assert("erro socket empty", g_ping->socket.family == AF_INET);
  mu_assert("erro socket empty", g_ping->socket.socktype == SOCK_RAW);
  mu_assert("erro socket empty", g_ping->socket.protocol == IPPROTO_ICMP);
  return 0;
}

static char *all_tests() {
  mu_run_test(test_socket);
  mu_run_test(test_sockets);
  return 0;
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  t_ftping pingdata;

  g_ping = &pingdata;

  setup();

  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  } else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
