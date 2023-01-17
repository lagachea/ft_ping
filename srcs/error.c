#include "ft_ping.h"

void printError(const char *fmt, const char *value) {
	dprintf(STDERR_FILENO, fmt, value);
}
