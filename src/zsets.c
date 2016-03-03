/* See LICENSE file for copyright and license details. */
#include "internals"

#include <errno.h>


int
zsets(z_t a, const char *str)
{
	unsigned long long int temp = 0;
	int neg = (*str == '-');
	const char *str_end;

	str += neg || (*str == '+');

	if (!*str) {
		errno = EINVAL;
		return -1;
	}
	for (str_end = str; *str_end; str_end++) {
		if (!isdigit(*str_end)) {
			errno = EINVAL;
			return -1;
		}
	}

	SET_SIGNUM(a, 0);

	switch ((str_end - str) % 19) {
		while (*str) {
#define X(n)\
		case n:\
			temp *= 10, temp += *str++ & 15;\
		X(0) X(18) X(17) X(16) X(15) X(14) X(13) X(12) X(11)
		X(10) X(9) X(8) X(7) X(6) X(5) X(4) X(3) X(2) X(1)
#undef X

			zmul(a, a, libzahl_const_1e19);
			zsetu(libzahl_tmp_str_num, temp);
			zadd(a, a, libzahl_tmp_str_num);
			temp = 0;
		}
	}

	if (neg)
		SET_SIGNUM(a, -zsignum(a));
	return 0;
}
