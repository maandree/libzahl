/* See LICENSE file for copyright and license details. */
#include "internals"

#include <stdlib.h>
#include <string.h>


void
zlsh(z_t a, z_t b, size_t bits)
{
	size_t i, chars, cbits;
	zahl_char_t carry[] = {0, 0};

	if (zzero(b)) {
		SET_SIGNUM(a, 0);
		return;
	}
	if (!bits) {
		if (a != b)
			zset(a, b);
		return;
	}

	chars = FLOOR_BITS_TO_CHARS(bits);
	bits = BITS_IN_LAST_CHAR(bits)
	cbits = BITS_PER_CHAR - 1 - bits;

	a->used = b->used + chars;
	if (a->alloced < a->used) {
		a->alloced = a->used;
		a->chars = realloc(a->chars, a->used * sizeof(*(a->chars)));
	}
	(a == b ? memmove : memcpy)(a->chars + chars, b->chars, a->used * sizeof(*(a->chars)));
	memset(a->chars, 0, chars * sizeof(*(a->chars)));

	for (i = chars; i < a->used; i++) {
		carry[~i & 1] = a->chars[i] >> cbits;
		a->chars[i] <<= bits;
		a->chars[i] |= carry[i & 1];
	}
	if (carry[i & 1]) {
		if (a->alloced == a->used) {
			a->alloced <<= 1;
			a->chars = realloc(a->chars, a->alloced * sizeof(*(a->chars)));
		}
		a->chars[i] = carry[i & 1];
		a->used++;
	}

	SET_SIGNUM(a, zsignum(b));
}
