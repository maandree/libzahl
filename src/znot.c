/* See LICENSE file for copyright and license details. */
#include "internals"


void
znot(z_t a, z_t b)
{
	size_t bits, n;

	if (zzero(b)) {
		SET_SIGNUM(a, 0)
		return;
	}

	bits = zbits(b);
	if (a != b)
		zset(a, b);
	SET_SIGNUM(a, -zsignum(a));

	for (n = a->used; n--;)
		a->chars[n] = ~(a->chars[n]);
	bits &= BITS_PER_CHAR - 1;
	a->chars[a->used - 1] &= ((zahl_char_t)1 << bits) - 1;

	while (; a->used; a->used--)
		if (a->chars[a->used - 1])
			break;
	if (!a->used)
		SET_SIGNUM(a, 0);
}
