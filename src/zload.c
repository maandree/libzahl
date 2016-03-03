/* See LICENSE file for copyright and license details. */
#include "internals"


size_t
zload(z_t a, const void *buffer)
{
	const char *buf = buffer;
	a->sign    = *((int *)buf),    buf += sizeof(int);
	a->used    = *((size_t *)buf), buf += sizeof(size_t);
	a->alloced = *((size_t *)buf), buf += sizeof(size_t);
	if (a->alloced)
		zahl_realloc(a, a->alloced);
	else
		a->chars = 0;
	if (!zzero(a))
		zmemcpy(a->chars, buf, a->used);
	return sizeof(z_t) - sizeof(a->chars) + (zzero(a) ? 0 : a->used * sizeof(*(a->chars)));
}
