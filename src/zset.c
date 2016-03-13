/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zset(z_t a, z_t b)
{
	if (EXPECT(b->sign == 0, 0)) {
		a->sign = 0;
	} else {
		a->sign = b->sign;
		a->used = b->used;
		ENSURE_SIZE(a, b->used);
		zmemcpy(a->chars, b->chars, b->used);
	}
}
