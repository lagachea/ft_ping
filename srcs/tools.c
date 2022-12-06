#include "ft_ping.h"
#include <stdio.h>
#include <unistd.h>

void	print_memory(const void *addr, size_t size)
{
	unsigned char *t = (unsigned char *)addr;
	size_t		i = 0;
	size_t		col;
	size_t		tmp = 0;
	size_t		cut = 16;

	while (i < size)
	{
		col = 0;
		tmp = i;
		while (col < cut)
		{
			printf("%.2x ", t[i]);
			i++;
			col++;
		}
		col = 0;
		i = tmp;
		while (col < cut && i < size) {
			unsigned char c;
			c = t[i++];
			printf("%c", (c > 31 && c < 127) ? c : '.');
			col++;
		}
		printf("\n");
	}
}
