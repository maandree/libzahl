/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zbset(z_t a, z_t b, size_t bit, int action)
{
	zahl_char_t x = 1;
	size_t chars;

	chars = FLOOR_BITS_TO_CHARS(bit);
	bit = BITS_IN_LAST_CHAR(bit);
	x <<= bit;

	SET(a, b);

	if (action) {
		if (zzero(a)) {
			a->used = 0;
			SET_SIGNUM(a, 1);
		}
		if (a->used <= chars) {
			if (a->alloced <= chars)
				zahl_realloc(a, chars + 1);
			zmemset(a->chars + a->used, 0, chars - a->used + 1);
		}
	}

	if (action > 0) {
		a->chars[chars] |= x;
		return;
	} else if (action < 0) {
		a->chars[chars] ^= x;
	} else if (a->used > chars) {
		a->chars[chars] &= ~x;
	}

	while (a->used && !a->chars[a->used - 1])
		a->used--;
	if (!a->used)
		SET_SIGNUM(a, 0);
}
