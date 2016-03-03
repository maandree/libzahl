/* See LICENSE file for copyright and license details. */
#include "internals"

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
	size_t read_total, n, chars = CEILING_BITS_TO_CHARS(bits);
	ssize_t read_just;
	uint32_t mask = 1;

	if (r->alloced < chars)
		zahl_realloc(r, chars);

	for (n = chars << LB_BITS_PER_CHAR; n;) {
		read_just = read(fd, (char *)(r->chars) + read_total, n);
		if (read_just < 0)
			FAILURE_JUMP();
		read_total += read_just;
		n -= read_just;
	}

	bit = BITS_IN_LAST_CHAR(bit)
	mask <<= bit;
	mask -= 1;

	r->chars[chars - 1] &= mask;
	for (n = chars; n--;) {
		if (r->chars[n]) {
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
		abort();
	}

	if (zzero(n)) {
		SET_SIGNUM(r, 0);
		return;
	}

	fd = open(pathname, O_RDONLY);

	switch (dist) {
	case QUASIUNIFORM:
		bits = zbits(n);
		zrand_get_random_bits(r, bits, fd);
		zadd(r, r, libzahl_const_1);
		zmul(r, r, n);
		zrsh(r, r, bits);
		break;

	case UNIFORM:
		bits = zbits(n);
		do
			zrand_get_random_bits(r, bits, fd);
		while (zcmp(r, n) > 0);
		break;

	default:
		abort();
	}

	close(fd);
}
