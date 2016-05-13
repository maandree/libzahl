/* See LICENSE file for copyright and license details. */
#include "internals.h"

#define tb  libzahl_tmp_pow_b
#define tc  libzahl_tmp_pow_c


void
zpow(z_t a, z_t b, z_t c)
{
	/*
	 * Exponentiation by squaring.
	 * 
	 * 7↑19 = 7↑10011₂ = 7↑2⁰ ⋅ 7↑2¹ ⋅ 7↑2⁴ where a↑2↑(n + 1) = (a↑2↑n)².
	 */

	/* TODO use zpowu when possible */

	size_t i, j, n, bits;
	zahl_char_t x;
	int neg;

	if (unlikely(zsignum(c) <= 0)) {
		if (zzero(c)) {
			if (check(zzero(b)))
				libzahl_failure(-ZERROR_0_POW_0);
			zsetu(a, 1);
		} else if (check(zzero(b))) {
			libzahl_failure(-ZERROR_DIV_0);
		} else {
			SET_SIGNUM(a, 0);
		}
		return;
	} else if (unlikely(zzero(b))) {
		SET_SIGNUM(a, 0);
		return;
	}

	bits = zbits(c);
	n = FLOOR_BITS_TO_CHARS(bits);

	neg = znegative(b) && zodd(c);
	zabs(tb, b);
	zset(tc, c);
	zsetu(a, 1);

	for (i = 0; i < n; i++) { /* Remember, n is floored. */
		x = tc->chars[i];
		for (j = BITS_PER_CHAR; j--; x >>= 1) {
			if (x & 1)
				zmul_ll(a, a, tb);
			zsqr_ll(tb, tb);
		}
	}
	x = tc->chars[i];
	for (; x; x >>= 1) {
		if (x & 1)
			zmul_ll(a, a, tb);
		zsqr_ll(tb, tb);
	}

	if (neg)
		zneg(a, a);
}
