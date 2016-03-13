/* See LICENSE file for copyright and license details. */
#include "internals.h"


size_t
zload(z_t a, const void *buffer)
{
	const char *buf = buffer;
	a->sign = *((const int *)buf),    buf += sizeof(int);
	a->used = *((const size_t *)buf), buf += sizeof(size_t);
	if (EXPECT(!!a->sign, 1)) {
		ENSURE_SIZE(a, a->used);
		zmemcpy(a->chars, (const zahl_char_t *)buf, a->used);
	}
	return sizeof(int) + sizeof(size_t) + (zzero(a) ? 0 : a->used * sizeof(zahl_char_t));
}
