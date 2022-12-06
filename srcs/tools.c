#include "ft_ping.h"
#include <stdio.h>
#include <unistd.h>

void	print_memory(const void *addr, size_t size)
{
	unsigned char *t = (unsigned char *)addr;
	size_t		i = 0;
	int			col;
	size_t		tmp = 0;
	int			cut = 16;

	while (i < size)
	{
		tmp = i;

		int count;
		for (count = 0; count < cut; count++){
			printf("%.2x ", t[count]);
		}
		col = -1;
		i = tmp;
		while (++col < cut && i < size) {
			unsigned char c;

			c = t[i++];
			printf("%c", (c > 31 && c < 127) ? c : '.');
		}
		printf("\n");
	}
}
