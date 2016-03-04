/* See LICENSE file for copyright and license details. */
#include "internals.h"


size_t
zload(z_t a, const void *buffer)
{
	const char *buf = buffer;
	size_t alloced;
	a->sign = *((const int *)buf),    buf += sizeof(int);
	a->used = *((const size_t *)buf), buf += sizeof(size_t);
	alloced = *((const size_t *)buf), buf += sizeof(size_t);
	if (alloced)
		ENSURE_SIZE(a, alloced);
	else
		a->chars = 0;
	if (!zzero(a))
		zmemcpy(a->chars, buf, a->used);
	return sizeof(z_t) - sizeof(a->chars) + (zzero(a) ? 0 : a->used * sizeof(*(a->chars)));
}
