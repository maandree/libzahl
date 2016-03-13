/* See LICENSE file for copyright and license details. */
#include "internals.h"
#include <stdio.h>


/* Find r such that r is a minimal power of 2 ≥ n. */
#define NEXT_2POW(n)\
	do if (likely((n & (~n + 1)) != n)) {\
		size_t x;\
		n |= n >> 1;\
		n |= n >> 2;\
		n |= n >> 4;\
		for (i = sizeof(n), x = 8; i; i >>= 1, x <<= 1)\
			n |= n >> x;\
		n += 1;\
	} while (0)


#if defined(__clang__)
static inline __attribute__((optnone)) size_t
clang_warkaround_next_2pow(size_t need)
{
	size_t i;
	NEXT_2POW(need);
	return need;
}
#endif


void
libzahl_realloc(z_t a, size_t need)
{
	size_t i;
	zahl_char_t *new;

#if defined(__clang__)
	/* https://llvm.org/bugs/show_bug.cgi?id=26930 */
	need = clang_warkaround_next_2pow(need);
#else
	NEXT_2POW(need);
#endif

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
