/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zsetu(z_t a, uint64_t b)
{
	if (!b) {
		SET_SIGNUM(a, 0);
		return;
	}
	ENSURE_SIZE(a, 1);
	SET_SIGNUM(a, 1);
	a->chars[0] = (zahl_char_t)b;
	a->used = 1;
}
