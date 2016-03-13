/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
znot(z_t a, z_t b)
{
	size_t bits, i;

	if (EXPECT(zzero(b), 0)) {
		SET_SIGNUM(a, 0);
		return;
	}

	bits = zbits(b);
	a->used = b->used;
	SET_SIGNUM(a, -zsignum(b));

	for (i = 0; i < a->used; i++)
		a->chars[i] = ~(b->chars[i]);
	bits = BITS_IN_LAST_CHAR(bits);
	if (bits)
		a->chars[a->used - 1] &= ((zahl_char_t)1 << bits) - 1;

	TRIM(a);
	if (!a->used)
		SET_SIGNUM(a, 0);
}
