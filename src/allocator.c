/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
libzahl_realloc(z_t a, size_t need)
{
	size_t i, new_size = 1;
	zahl_char_t *new;

	new_size <<= i = libzahl_msb_nz_zu(need);
	if (likely(new_size != need)) {
		i += 1;
		new_size <<= 1;
	}

	if (likely(libzahl_pool_n[i])) {
		libzahl_pool_n[i]--;
		new = libzahl_pool[i][libzahl_pool_n[i]];
		zmemcpy(new, a->chars, a->alloced);
		zfree(a);
		a->chars = new;
	} else {
		a->chars = realloc(a->chars, (new_size + ZAHL_FLUFF) * sizeof(zahl_char_t));
		if (check(!a->chars))
			libzahl_memfailure();
	}
	a->alloced = new_size;
}
