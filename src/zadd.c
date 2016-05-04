/* See LICENSE file for copyright and license details. */
#include "internals.h"


static inline void
zadd_impl_4(z_t a, z_t b, z_t c, size_t n)
{
	zahl_char_t carry = 0, tcarry;
	size_t i;

	for (i = 0; i < n; i++) {
		tcarry = libzahl_add_overflow(a->chars + i, b->chars[i], c->chars[i]);
		carry = tcarry | (zahl_char_t)libzahl_add_overflow(a->chars + i, a->chars[i], carry);
	}
	while (carry) {
		carry = libzahl_add_overflow(a->chars + i, a->chars[i], 1);
		i++;
	}

	if (a->used < i)
		a->used = i;
}

static inline void
zadd_impl_3(z_t a, z_t b, size_t n)
{
	zadd_impl_4(a, a, b, n);
}

static inline void
libzahl_zadd_unsigned(z_t a, z_t b, z_t c)
{
	size_t size, n;

	if (unlikely(zzero(b))) {
		zabs(a, c);
		return;
	} else if (unlikely(zzero(c))) {
		zabs(a, b);
		return;
	}

	size = MAX(b->used, c->used);
	n = b->used + c->used - size;

	ENSURE_SIZE(a, size + 1);
	a->chars[size] = 0;

	if (a == b) {
		if (a->used < c->used) {
			n = c->used;
			zmemset(a->chars + a->used, 0, n - a->used);
		}
		zadd_impl_3(a, c, n);
	} else if (unlikely(a == c)) {
		if (a->used < b->used) {
			n = b->used;
			zmemset(a->chars + a->used, 0, n - a->used);
		}
		zadd_impl_3(a, b, n);
	} else if (likely(b->used > c->used)) {
		zmemcpy(a->chars + n, b->chars + n, size - n);
		a->used = size;
		zadd_impl_4(a, b, c, n);
	} else {
		zmemcpy(a->chars + n, c->chars + n, size - n);
		a->used = size;
		zadd_impl_4(a, b, c, n);
	}

	SET_SIGNUM(a, 1);
}

void
zadd_unsigned(z_t a, z_t b, z_t c)
{
	libzahl_zadd_unsigned(a, b, c);
}

void
zadd_unsigned_assign(z_t a, z_t b)
{
	size_t size, n;

	if (unlikely(zzero(a))) {
		zabs(a, b);
		return;
	} else if (unlikely(zzero(b))) {
		return;
	}

	size = MAX(a->used, b->used);
	n = a->used + b->used - size;

	ENSURE_SIZE(a, size + 1);
	a->chars[size] = 0;

	if (a->used < b->used) {
		n = b->used;
		zmemset(a->chars + a->used, 0, n - a->used);
	}
	zadd_impl_3(a, b, n);

	SET_SIGNUM(a, 1);
}

void
zadd(z_t a, z_t b, z_t c)
{
	if (unlikely(zzero(b))) {
		SET(a, c);
	} else if (unlikely(zzero(c))) {
		SET(a, b);
	} else if (unlikely(znegative(b))) {
		if (znegative(c)) {
			libzahl_zadd_unsigned(a, b, c);
			SET_SIGNUM(a, -zsignum(a));
		} else {
			zsub_unsigned(a, c, b);
		}
	} else if (unlikely(znegative(c))) {
		zsub_unsigned(a, b, c);
	} else {
		libzahl_zadd_unsigned(a, b, c);
	}
}
