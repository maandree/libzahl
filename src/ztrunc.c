/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
ztrunc(z_t a, z_t b, size_t bits)
{
	zahl_char_t mask = 1;
	size_t chars;

	if (EXPECT(zzero(b), 0)) {
		SET_SIGNUM(a, 0);
		return;
	}

	chars = CEILING_BITS_TO_CHARS(bits);
	a->sign = b->sign;
	a->used = MIN(chars, b->used);
	if (EXPECT(a->used < chars, 0))
		bits = 0;
	if (EXPECT(a != b, 1)) {
		ENSURE_SIZE(a, a->used);
		zmemcpy(a->chars, b->chars, a->used);
	}
	bits = BITS_IN_LAST_CHAR(bits);
	if (EXPECT(!!bits, 1)) {
		mask <<= bits;
		mask -= 1;
		a->chars[a->used - 1] &= mask;
	}

	TRIM(a);
	if (!a->used)
		SET_SIGNUM(a, 0);
}
