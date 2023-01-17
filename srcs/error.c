#include "ft_ping.h"

int printError(const char *fmt, ...) {
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = vdprintf(STDERR_FILENO, fmt, args);
	va_end(args);

	return ret;
}
