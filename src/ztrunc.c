/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
ztrunc(z_t a, z_t b, size_t bits)
{
	size_t chars;

	if (unlikely(zzero(b))) {
		SET_SIGNUM(a, 0);
		return;
	}

	chars = CEILING_BITS_TO_CHARS(bits);
	a->used = MIN(chars, b->used);
	if (unlikely(a->used < chars))
		bits = 0;
	if (likely(a != b)) {
		a->sign = b->sign;
		ENSURE_SIZE(a, a->used);
		zmemcpy(a->chars, b->chars, a->used);
	}
	bits = BITS_IN_LAST_CHAR(bits);
	if (likely(bits))
		a->chars[a->used - 1] &= ((zahl_char_t)1 << bits) - 1;

	TRIM_AND_ZERO(a);
}
