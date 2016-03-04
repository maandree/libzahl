/* See LICENSE file for copyright and license details. */
#include "internals.h"


size_t
zload(z_t a, const void *buffer)
{
	const char *buf = buffer;
	a->sign    = *((const int *)buf),    buf += sizeof(int);
	a->used    = *((const size_t *)buf), buf += sizeof(size_t);
	a->alloced = *((const size_t *)buf), buf += sizeof(size_t);
	if (a->alloced)
		zahl_realloc(a, a->alloced);
	else
		a->chars = 0;
	if (!zzero(a))
		zmemcpy(a->chars, buf, a->used);
	return sizeof(z_t) - sizeof(a->chars) + (zzero(a) ? 0 : a->used * sizeof(*(a->chars)));
}
