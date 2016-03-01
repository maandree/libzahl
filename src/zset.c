/* See LICENSE file for copyright and license details. */
#include "internals"

#include <stdlib.h>
#include <string.h>


void
zset(z_t a, z_t b)
{
	if (zzero(b)) {
		SET_SIGNUM(a, 0);
	} else {
		if (a->alloced < b->alloced) {
			a->alloced = b->alloced;
			a->chars = realloc(a->chars, b->alloced * sizeof(*(a->chars)));
		}
		a->sign = b->sign;
		a->used = b->used;
		memcpy(a->chars, b->chars, b->used * sizeof(*(a->chars)));
	}
}
