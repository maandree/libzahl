/* See LICENSE file for copyright and license details. */
#include "internals.h"


static inline O2 void
zor_impl(zahl_char_t *restrict a, const zahl_char_t *restrict b, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		a[i] |= b[i];
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

	m = MAX(b->used, c->used);
	n = b->used + c->used - m;

	ENSURE_SIZE(a, m);

	if (a == b) {
		if (a->used < c->used)
			zmemcpy(a->chars + n, c->chars + n, m - n);
		zor_impl(a->chars, c->chars, n);
	} else if (unlikely(a == c)) {
		if (a->used < b->used)
			zmemcpy(a->chars + n, b->chars + n, m - n);
		zor_impl(a->chars, b->chars, n);
	} else  if (m == b->used) {
		zmemcpy(a->chars, b->chars, m);
		zor_impl(a->chars, c->chars, n);
	} else {
		zmemcpy(a->chars, c->chars, m);
		zor_impl(a->chars, b->chars, n);
	}

	a->used = m;
	SET_SIGNUM(a, zpositive2(b, c) * 2 - 1);
}
