#include "ft_ping.h"
#include "libft.h"
#include <arpa/inet.h>
#include <netinet/in.h>

int	main(void)
{
	struct timeval  tv;
	struct timezone  tz;
	in_addr_t  add;
	char buf[INET_ADDRSTRLEN];
	int ret;

	ft_memset(&tv, 0, sizeof(tv));
	ft_memset(&add, 0, sizeof(add));
	ft_memset(&tz, 0, sizeof(tz));
	printf("%ld %ld\n", tv.tv_sec, tv.tv_usec);
	printf("%d %d\n", tz.tz_dsttime, tz.tz_minuteswest);

	ret = gettimeofday(&tv, &tz);
	printf("ret = %d\n", ret);
	printf("%ld %ld\n", tv.tv_sec, tv.tv_usec);
	printf("%d %d\n", tz.tz_dsttime, tz.tz_minuteswest);

	inet_ntop(AF_INET, &add, buf, INET_ADDRSTRLEN);
	printf("%s\n", buf);

	return 0;
}
