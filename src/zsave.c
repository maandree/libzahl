/* See LICENSE file for copyright and license details. */
#include "internals"


size_t
zsave(z_t a, void *buffer)
{
	if (buffer) {
		char *buf = buffer;
		*((int *)buf)    = a->sign,    buf += sizeof(int);
		*((size_t *)buf) = a->used,    buf += sizeof(size_t);
		*((size_t *)buf) = a->alloced, buf += sizeof(size_t);
		if (!zzero(a))
			zmemcpy(buf, a->chars, a->used);
	}
	return sizeof(z_t) - sizeof(a->chars) + (zzero(a) ? 0 : a->used * sizeof(*(a->chars)));
}
