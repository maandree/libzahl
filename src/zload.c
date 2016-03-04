/* See LICENSE file for copyright and license details. */
#include "internals.h"


size_t
zload(z_t a, const void *buffer)
{
	const char *buf = buffer;
	a->sign = *((const int *)buf),    buf += sizeof(int);
	a->used = *((const size_t *)buf), buf += sizeof(size_t);
	a->alloced = 0;
	if (a->sign)
		ENSURE_SIZE(a, a->used);
	else
		a->chars = 0;
	if (!zzero(a))
		zmemcpy(a->chars, buf, a->used);
	return sizeof(int) + sizeof(size_t) + (zzero(a) ? 0 : a->used * sizeof(zahl_char_t));
}
