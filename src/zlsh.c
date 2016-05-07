/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zlsh(z_t a, z_t b, size_t bits)
{
	size_t i, chars, cbits;
	zahl_char_t carry = 0, tcarry;

	if (unlikely(zzero(b))) {
		SET_SIGNUM(a, 0);
		return;
	}

	chars = FLOOR_BITS_TO_CHARS(bits);
	bits = BITS_IN_LAST_CHAR(bits);
	cbits = BITS_PER_CHAR - bits;

	ENSURE_SIZE(a, b->used + chars + 1);
	if (likely(a == b)) {
		zmemmoveb(a->chars + chars, b->chars, b->used);
	} else {
		zmemcpy(a->chars + chars, b->chars, b->used);
	}
	zmemset_precise(a->chars, 0, chars);
	a->used = b->used + chars;

	if (likely(bits)) { /* This if statement is very important in C. */
		for (i = chars; i < a->used; i++) {
			tcarry = a->chars[i] >> cbits;
			a->chars[i] <<= bits;
			a->chars[i] |= carry;
			carry = tcarry;
		}
		if (carry)
			a->chars[a->used++] = carry;
	}

	SET_SIGNUM(a, zsignum(b));
}
