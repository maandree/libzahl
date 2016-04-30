/* See LICENSE file for copyright and license details. */
#include "internals.h"


O2 static inline void
zxor_impl_3(register zahl_char_t *restrict a,
	    register const zahl_char_t *restrict b, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		a[i] ^= b[i];
}

static inline void
zxor_impl_5(register zahl_char_t *restrict a,
	    register const zahl_char_t *restrict b, size_t n,
	    register const zahl_char_t *restrict c, size_t m)
{
	size_t i;
	for (i = 0; i < n; i++)
		a[i] = b[i] ^ c[i];
	for (; i < m; i++)
		a[i] = c[i];
}

void
zxor(z_t a, z_t b, z_t c)
{
	size_t n, m, bn, cn;
	const zahl_char_t *restrict bc;
	const zahl_char_t *restrict cc;

	if (unlikely(zzero(b))) {
		SET(a, c);
		return;
	} else if (unlikely(zzero(c))) {
		SET(a, b);
		return;
	}

	bn = b->used;
	bc = b->chars;
	cn = c->used;
	cc = c->chars;

	MIN_MAX_1(n, m, bn, cn);
	ENSURE_SIZE(a, m);

	if (a == b) {
		zxor_impl_3(a->chars, cc, n);
		if (a->used < cn)
			zmemcpy_range(a->chars, cc, n, m);
	} else if (unlikely(a == c)) {
		zxor_impl_3(a->chars, bc, n);
		if (a->used < bn)
			zmemcpy_range(a->chars, bc, n, m);
	} else if (m == bn) {
		zxor_impl_5(a->chars, cc, n, bc, m);
	} else {
		zxor_impl_5(a->chars, bc, n, cc, m);
	}

	a->used = m;
	TRIM_AND_SIGN(a, 1 - 2 * ((zsignum(b) ^ zsignum(c)) < 0));
}
