/* See LICENSE file for copyright and license details. */
#include "internals.h"

#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifndef FAST_RANDOM_PATHNAME
# define FAST_RANDOM_PATHNAME  "/dev/urandom"
#endif

#ifndef SECURE_RANDOM_PATHNAME
# define SECURE_RANDOM_PATHNAME  "/dev/random"
#endif


static void
zrand_libc_rand(void *out, size_t n, void *statep)
{
	static char inited = 0;

	unsigned int ri;
	double rd;
	unsigned char *buf = out;

	if (!inited) {
		inited = 1;
		srand((unsigned)((intptr_t)out | time(NULL)));
	}

	while (n--) {
		ri = (unsigned)rand();
		rd = (double)ri / ((double)RAND_MAX + 1);
#ifdef GOOD_RAND
		rd *= 256 * 256;
		ri = (unsigned int)rd;
		buf[n] = (unsigned char)((ri >> 0) & 255);
		if (!n--) break;
		buf[n] = (unsigned char)((ri >> 8) & 255);
#else
		rd *= 256;
		buf[n] = (unsigned char)rd;
#endif
	}

	(void) statep;
}

static void
zrand_libc_rand48(void *out, size_t n, void *statep)
{
	static char inited = 0;

	long int r0, r1;
	unsigned char *buf = out;

	if (!inited) {
		inited = 1;
		srand48((intptr_t)out | time(NULL));
	}

	while (n--) {
		r0 = lrand48() & 15;
		r1 = lrand48() & 15;
		buf[n] = (unsigned char)((r0 << 4) | r1);
	}

	(void) statep;
}

static void
zrand_libc_random(void *out, size_t n, void *statep)
{
	static char inited = 0;

	long int ri;
	unsigned char *buf = out;

	if (!inited) {
		inited = 1;
		srandom((unsigned)((intptr_t)out | time(NULL)));
	}

	while (n--) {
		ri = random();
		buf[n] = (unsigned char)((ri >>  0) & 255);
		if (!n--) break;
		buf[n] = (unsigned char)((ri >>  8) & 255);
		if (!n--) break;
		buf[n] = (unsigned char)((ri >> 16) & 255);
	}

	(void) statep;
}

static void
zrand_fd(void *out, size_t n, void *statep)
{
	int fd = *(int *)statep;
	ssize_t read_just;
	size_t read_total = 0;
	char *buf = out;

	while (n) {
		read_just = read(fd, buf + read_total, n);
		if (check(read_just < 0))
			libzahl_failure(errno);
		read_total += (size_t)read_just;
		n -= (size_t)read_just;
	}
}

static void
zrand_get_random_bits(z_t r, size_t bits, void (*fun)(void *, size_t, void *), void *statep)
{
	size_t n, chars = CEILING_BITS_TO_CHARS(bits);
	zahl_char_t mask = 1;

	ENSURE_SIZE(r, chars);

	fun(r->chars, chars * sizeof(zahl_char_t), statep);

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
#define RANDOM_UNIFORM(RETRY)\
	do {\
		if (check(znegative(n)))\
			libzahl_failure(-ZERROR_NEGATIVE);\
		bits = zbits(n);\
		do\
			zrand_get_random_bits(r, bits, random_fun, statep);\
		while (RETRY && unlikely(zcmpmag(r, n) > 0));\
	} while (0)


	const char *pathname = 0;
	size_t bits;
	int fd = -1;
	void *statep = 0;
	void (*random_fun)(void *, size_t, void *) = &zrand_fd;

        switch (dev) {
	case FAST_RANDOM:
		pathname = FAST_RANDOM_PATHNAME;
		break;
	case SECURE_RANDOM:
		pathname = SECURE_RANDOM_PATHNAME;
		break;
	case LIBC_RAND_RANDOM:
		random_fun = &zrand_libc_rand;
		break;
	case DEFAULT_RANDOM:
	case FASTEST_RANDOM:
	case LIBC_RANDOM_RANDOM:
		random_fun = &zrand_libc_random;
		break;
	case LIBC_RAND48_RANDOM:
		random_fun = &zrand_libc_rand48;
		break;
	default:
		libzahl_failure(EINVAL);
	}

	if (unlikely(zzero(n))) {
		SET_SIGNUM(r, 0);
		return;
	}

	if (pathname) {
		fd = open(pathname, O_RDONLY);
		if (check(fd < 0))
			libzahl_failure(errno);
		statep = &fd;
	}

	switch (dist) {
	case QUASIUNIFORM:
		RANDOM_UNIFORM(0);
		zadd(r, r, libzahl_const_1);
		zmul(r, r, n);
		zrsh(r, r, bits);
		break;

	case UNIFORM:
		RANDOM_UNIFORM(1);
		break;

	case MODUNIFORM:
		RANDOM_UNIFORM(0);
		if (unlikely(zcmpmag(r, n) > 0))
		        zsub(r, r, n);
		break;

	default:
#if !defined(ZAHL_UNSAFE)
		libzahl_failure(EINVAL);
#endif
		break;
	}

	if (fd >= 0)
		close(fd);
}
