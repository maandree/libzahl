/* See LICENSE file for copyright and license details. */
#include "internals.h"


O2 static inline void
zor_impl_3(register zahl_char_t *restrict a,
	    register const zahl_char_t *restrict b, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		a[i] |= b[i];
}

static inline void
zor_impl_5(register zahl_char_t *restrict a,
	    register const zahl_char_t *restrict b, size_t n,
	    register const zahl_char_t *restrict c, size_t m)
{
	size_t i;
	for (i = 0; i < n; i++)
		a[i] = b[i] | c[i];
	for (; i < m; i++)
		a[i] = c[i];
}

void
zor(z_t a, z_t b, z_t c)
{
	size_t n, m;

	if (unlikely(zzero(b))) {
		SET(a, c);
		return;
	} else if (unlikely(zzero(c))) {
		SET(a, b);
		return;
	}

	MIN_MAX_1(n, m, b->used, c->used);
	ENSURE_SIZE(a, m);

	if (a == b) {
		zor_impl_3(a->chars, c->chars, n);
		if (a->used < c->used)
			zmemcpy_range(a->chars, c->chars, n, m);
	} else if (unlikely(a == c)) {
		zor_impl_3(a->chars, b->chars, n);
		if (a->used < b->used)
			zmemcpy_range(a->chars, b->chars, n, m);
	} else  if (m == b->used) {
		zor_impl_5(a->chars, c->chars, n, b->chars, m);
	} else {
		zor_impl_5(a->chars, b->chars, n, c->chars, m);
	}

	a->used = m;
	SET_SIGNUM(a, zpositive2(b, c) * 2 - 1);
}
