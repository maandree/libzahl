/* See LICENSE file for copyright and license details. */
#include "internals.h"


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
		ZMEM_2OP_PRECISE(a->chars, a->chars, cc, n, ^);
		if (a->used < cn)
			zmemcpy_range(a->chars, cc, n, m);
	} else if (unlikely(a == c)) {
		ZMEM_2OP_PRECISE(a->chars, a->chars, bc, n, ^);
		if (a->used < bn)
			zmemcpy_range(a->chars, bc, n, m);
	} else if (m == bn) {
		ZMEM_2OP(a->chars, c->chars, b->chars, n, ^);
		zmemcpy_range(a->chars, b->chars, n, m);
	} else {
		ZMEM_2OP(a->chars, b->chars, c->chars, n, ^);
		zmemcpy_range(a->chars, c->chars, n, m);
	}

	a->used = m;
	TRIM_AND_SIGN(a, 1 - 2 * ((zsignum(b) ^ zsignum(c)) < 0));
}
