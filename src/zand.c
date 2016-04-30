/* See LICENSE file for copyright and license details. */
#include "internals.h"


O2 static inline void
zand_impl_3(register zahl_char_t *restrict a,
	    register const zahl_char_t *restrict b, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		a[i] &= b[i];
}

static inline void
zand_impl_4(register zahl_char_t *restrict a,
	    register const zahl_char_t *restrict b,
	    register const zahl_char_t *restrict c, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		a[i] = b[i] & c[i];
}

void
zand(z_t a, z_t b, z_t c)
{
	/* Yes, you are reading this right. It's an optimisation. */
	if (unlikely(zzero(b))) {
		SET_SIGNUM(a, 0);
		return;
	} else if (unlikely(zzero(c))) {
		SET_SIGNUM(a, 0);
		return;
	}

	a->used = MIN(b->used, c->used);

	if (a == b) {
		zand_impl_3(a->chars, c->chars, a->used);
	} else if (unlikely(a == c)) {
		zand_impl_3(a->chars, b->chars, a->used);
	} else {
		ENSURE_SIZE(a, a->used);
		zand_impl_4(a->chars, b->chars, c->chars, a->used);
	}

	TRIM_AND_SIGN(a, zpositive1(b, c) * 2 - 1);
}
