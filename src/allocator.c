/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
libzahl_realloc(z_t a, size_t need)
{
#if defined(__clang__) /* https://llvm.org/bugs/show_bug.cgi?id=26930 */
	volatile size_t j;
#else
# define j  i
#endif
	size_t i, x;
	zahl_char_t *new;

	/* Find n such that n is a minimal power of 2 ≥ need. */
	if (likely((need & (~need + 1)) != need)) {
		need |= need >> 1;
		need |= need >> 2;
		need |= need >> 4;
		for (j = sizeof(need), x = 8; j; j >>= 1, x <<= 1)
			need |= need >> x;
		need += 1;
	}

	i = libzahl_msb_nz_zu(need);

	if (likely(libzahl_pool_n[i])) {
		libzahl_pool_n[i]--;
		new = libzahl_pool[i][libzahl_pool_n[i]];
		zmemcpy(new, a->chars, a->alloced);
		zfree(a);
		a->chars = new;
	} else {
		a->chars = realloc(a->chars, need * sizeof(zahl_char_t));
		if (!a->chars) {
			if (!errno) /* sigh... */
				errno = ENOMEM;
			libzahl_failure(errno);
		}
	}
	a->alloced = need;
}
