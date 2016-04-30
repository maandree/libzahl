/* See LICENSE file for copyright and license details. */
#include "internals.h"


static inline void
zsub_impl(z_t a, z_t b, size_t n)
{
	zahl_char_t carry = 0, tcarry;
	size_t i;

	for (i = 0; i < n; i++) {
		tcarry = carry ? (a->chars[i] <= b->chars[i]) : (a->chars[i] < b->chars[i]);
		a->chars[i] -= b->chars[i];
		a->chars[i] -= carry;
		carry = tcarry;
	}

	if (carry) {
		while (!a->chars[i])
			a->chars[i++] = ZAHL_CHAR_MAX;
		if (a->chars[i] == 1)
			a->used--;
		else
			a->chars[i] -= 1;
	}
}

static inline void
libzahl_zsub_unsigned(z_t a, z_t b, z_t c)
{
	int magcmp;
	size_t n;

	if (unlikely(zzero(b))) {
		zabs(a, c);
		zneg(a, a);
		return;
	} else if (unlikely(zzero(c))) {
		zabs(a, b);
		return;
	}

	magcmp = zcmpmag(b, c);
	if (unlikely(magcmp <= 0)) {
		if (unlikely(magcmp == 0)) {
			SET_SIGNUM(a, 0);
			return;
		}
		n = b->used;
		if (a == b) {
			zset(libzahl_tmp_sub, b);
			SET(a, c);
			zsub_impl(a, libzahl_tmp_sub, n);
		} else {
			SET(a, c);
			zsub_impl(a, b, n);
		}
	} else {
		n = c->used;
		if (unlikely(a == c)) {
			zset(libzahl_tmp_sub, c);
			SET(a, b);
			zsub_impl(a, libzahl_tmp_sub, n);
		} else {
			SET(a, b);
			zsub_impl(a, c, n);
		}
	}

	SET_SIGNUM(a, magcmp);
}

void
zsub_unsigned(z_t a, z_t b, z_t c)
{
	libzahl_zsub_unsigned(a, b, c);
}

void
zsub_nonnegative_assign(z_t a, z_t b)
{
	if (unlikely(zzero(b)))
		zabs(a, a);
	else if (unlikely(!zcmpmag(a, b)))
		SET_SIGNUM(a, 0);
	else
		zsub_impl(a, b, b->used);
}

void
zsub_positive_assign(z_t a, z_t b)
{
	zsub_impl(a, b, b->used);
}

void
zsub(z_t a, z_t b, z_t c)
{
	if (unlikely(zzero(b))) {
		zneg(a, c);
	} else if (unlikely(zzero(c))) {
		SET(a, b);
	} else if (unlikely(znegative(b))) {
		if (znegative(c)) {
			libzahl_zsub_unsigned(a, c, b);
		} else {
			zadd_unsigned(a, b, c);
			SET_SIGNUM(a, -zsignum(a));
		}
	} else if (znegative(c)) {
		zadd_unsigned(a, b, c);
	} else {
		libzahl_zsub_unsigned(a, b, c);
	}
}
