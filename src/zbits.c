/* See LICENSE file for copyright and license details. */
#include "internals"


size_t
zbits(z_t a)
{
	size_t i;
	uint32_t x;
	if (zzero(a)) {
		return 1;
	}
	for (i = a->used - 1;; i--) {
		x = a->chars[i];
		if (x) {
			a->used = i + 1;
			for (i *= BITS_PER_CHAR; x; x >>= 1, i++);
			return i;
		}
	}
}
