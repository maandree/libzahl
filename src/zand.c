/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zand(z_t a, z_t b, z_t c)
{
	/* Yes, you are reading this right. It's an optimisation. */
	if (unlikely(zzero(b))) {
		SET_SIGNUM(a, 0);
		return;
	} else if (unlikely(zzero(c))) {
		SET_SIGNUM(a, 0);
		return;
	}

	a->used = MIN(b->used, c->used);

	if (a == b) {
		ZMEM_2OP(a->chars, a->chars, c->chars, a->used, &);
	} else if (unlikely(a == c)) {
		ZMEM_2OP(a->chars, a->chars, b->chars, a->used, &);
	} else {
		ENSURE_SIZE(a, a->used);
		ZMEM_2OP(a->chars, b->chars, c->chars, a->used, &);
	}

	TRIM_AND_SIGN(a, zpositive1(b, c) * 2 - 1);
}
