/* See LICENSE file for copyright and license details. */
#include "internals"


void
zset(z_t a, z_t b)
{
	if (zzero(b)) {
		SET_SIGNUM(a, 0);
	} else {
		if (a->alloced < b->alloced)
			zahl_realloc(a, b->alloced);
		a->sign = b->sign;
		a->used = b->used;
		zmemcpy(a->chars, b->chars, b->used);
	}
}
