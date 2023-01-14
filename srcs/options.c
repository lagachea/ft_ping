#include "ft_ping.h"

void parseOpts(int ac, char **av) {
	// Parse host from arg
	g_ping->node = av[ac - 1];
}
