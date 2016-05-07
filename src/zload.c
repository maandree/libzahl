/* See LICENSE file for copyright and license details. */
#include "internals.h"


size_t
zload(z_t a, const void *buffer)
{
	const char *buf = buffer;
	a->sign = (int)*((const long *)buf), buf += sizeof(long);
	a->used = *((const size_t *)buf),    buf += sizeof(size_t);
	if (likely(a->sign)) {
		ENSURE_SIZE(a, a->used);
		zmemcpy(a->chars, (const zahl_char_t *)buf, a->used);
	}
	return sizeof(long) + sizeof(size_t) +
		(zzero(a) ? 0 : ((a->used + 3) & (size_t)~3) * sizeof(zahl_char_t));
}
