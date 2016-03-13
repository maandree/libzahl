/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zrsh(z_t a, z_t b, size_t bits)
{
	size_t i, chars, cbits;

	if (unlikely(!bits)) {
		SET(a, b);
		return;
	}

	chars = FLOOR_BITS_TO_CHARS(bits);

	if (unlikely(zzero(b) || chars >= b->used || zbits(b) <= bits)) {
		SET_SIGNUM(a, 0);
		return;
	}

	bits = BITS_IN_LAST_CHAR(bits);
	cbits = BITS_PER_CHAR - bits;

	if (likely(chars) && likely(a == b)) {
		a->used -= chars;
		zmemmove(a->chars, a->chars + chars, a->used);
	} else if (unlikely(a != b)) {
		a->used = b->used - chars;
		ENSURE_SIZE(a, a->used);
		zmemcpy(a->chars, b->chars + chars, a->used);
	}

	if (unlikely(bits)) { /* This if statement is very important in C. */
		a->chars[0] >>= bits;
		for (i = 1; i < a->used; i++) {
			a->chars[i - 1] |= a->chars[i] << cbits;
			a->chars[i] >>= bits;
		}
		TRIM_NONZERO(a);
	}

	SET_SIGNUM(a, zsignum(b));
}
