/* See LICENSE file for copyright and license details. */
#include "internals"

#include <stdlib.h>
#include <string.h>


size_t
zload(z_t a, const void *buffer)
{
	const char *buf = buffer;
	a->sign    = *((int *)buf),    buf += sizeof(int);
	a->used    = *((size_t *)buf), buf += sizeof(size_t);
	a->alloced = *((size_t *)buf), buf += sizeof(size_t);
	if (a->alloced) {
		a->chars = realloc(a->chars, a->alloced * sizeof(*(a->chars)));
	} else {
		a->chars = 0;
	}
	if (a->sign) {
		memcpy(a->chars, buf, a->used * sizeof(*(a->chars)));
	}
	return sizeof(z_t) - sizeof(a->chars) + (a->sign ? a->used * sizeof(*(a->chars)) : 0);
}
