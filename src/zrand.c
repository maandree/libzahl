/* See LICENSE file for copyright and license details. */
#include "internals.h"

#include <fcntl.h>
#include <unistd.h>

#ifndef FAST_RANDOM_PATHNAME
# define FAST_RANDOM_PATHNAME  "/dev/urandom"
#endif

#ifndef SECURE_RANDOM_PATHNAME
# define SECURE_RANDOM_PATHNAME  "/dev/random"
#endif


static void
zrand_get_random_bits(z_t r, size_t bits, int fd)
{
	size_t read_total = 0, n, chars = CEILING_BITS_TO_CHARS(bits);
	ssize_t read_just;
	zahl_char_t mask = 1;
	char *buf;

	ENSURE_SIZE(r, chars);
	buf = (char *)(r->chars);

	for (n = chars * sizeof(zahl_char_t); n;) {
		read_just = read(fd, buf + read_total, n);
		if (unlikely(read_just < 0))
			libzahl_failure(errno);
		read_total += (size_t)read_just;
		n -= (size_t)read_just;
	}

	bits = BITS_IN_LAST_CHAR(bits);
	mask <<= bits;
	mask -= 1;

	r->chars[chars - 1] &= mask;
	for (n = chars; n--;) {
		if (likely(r->chars[n])) {
			r->used = n + 1;
			SET_SIGNUM(r, 1);
			return;
		}
	}
        SET_SIGNUM(r, 0);
}

void
zrand(z_t r, enum zranddev dev, enum zranddist dist, z_t n)
{
	const char *pathname = 0;
	size_t bits;
	int fd;

        switch (dev) {
	case FAST_RANDOM:
		pathname = FAST_RANDOM_PATHNAME;
		break;
	case SECURE_RANDOM:
		pathname = SECURE_RANDOM_PATHNAME;
		break;
	default:
		libzahl_failure(EINVAL);
	}

	if (unlikely(zzero(n))) {
		SET_SIGNUM(r, 0);
		return;
	}

	fd = open(pathname, O_RDONLY);
	if (unlikely(fd < 0))
		libzahl_failure(errno);

	switch (dist) {
	case QUASIUNIFORM:
		if (unlikely(znegative(n)))
			libzahl_failure(-ZERROR_NEGATIVE);
		bits = zbits(n);
		zrand_get_random_bits(r, bits, fd);
		zadd(r, r, libzahl_const_1);
		zmul(r, r, n);
		zrsh(r, r, bits);
		break;

	case UNIFORM:
		if (unlikely(znegative(n)))
			libzahl_failure(-ZERROR_NEGATIVE);
		bits = zbits(n);
		do
			zrand_get_random_bits(r, bits, fd);
		while (unlikely(zcmpmag(r, n) > 0));
		break;

	case MODUNIFORM:
		if (unlikely(znegative(n)))
			libzahl_failure(-ZERROR_NEGATIVE);
		zrand_get_random_bits(r, zbits(n), fd);
		if (unlikely(zcmpmag(r, n) > 0))
		        zsub(r, r, n);
		break;

	default:
		libzahl_failure(EINVAL);
	}

	close(fd);
}
