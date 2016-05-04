/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
znot(z_t a, z_t b)
{
	size_t bits;

	if (unlikely(zzero(b))) {
		SET_SIGNUM(a, 0);
		return;
	}

	bits = zbits(b);
	a->used = b->used;
	SET_SIGNUM(a, -zsignum(b));

	ZMEM_1OP(a->chars, b->chars, a->used, ~);
	bits = BITS_IN_LAST_CHAR(bits);
	if (bits)
		a->chars[a->used - 1] &= ((zahl_char_t)1 << bits) - 1;

	TRIM_AND_ZERO(a);
}
