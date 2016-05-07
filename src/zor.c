/* See LICENSE file for copyright and license details. */
#include "internals.h"


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
		ZMEM_2OP_PRECISE(a->chars, a->chars, c->chars, n, |);
		if (a->used < c->used)
			zmemcpy_range(a->chars, c->chars, n, m);
	} else if (unlikely(a == c)) {
		ZMEM_2OP_PRECISE(a->chars, a->chars, b->chars, n, |);
		if (a->used < b->used)
			zmemcpy_range(a->chars, b->chars, n, m);
	} else  if (m == b->used) {
		ZMEM_2OP(a->chars, c->chars, b->chars, n, |);
		zmemcpy_range(a->chars, b->chars, n, m);
	} else {
		ZMEM_2OP(a->chars, b->chars, c->chars, n, |);
		zmemcpy_range(a->chars, c->chars, n, m);
	}

	a->used = m;
	SET_SIGNUM(a, zpositive2(b, c) * 2 - 1);
}
