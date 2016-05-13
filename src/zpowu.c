/* See LICENSE file for copyright and license details. */
#include "internals.h"

#define tb  libzahl_tmp_pow_b


void
zpowu(z_t a, z_t b, unsigned long long int c)
{
	int neg;

	if (unlikely(!c)) {
		if (check(zzero(b)))
			libzahl_failure(-ZERROR_0_POW_0);
		zsetu(a, 1);
		return;
	} else if (unlikely(zzero(b))) {
		SET_SIGNUM(a, 0);
		return;
	}

	neg = znegative(b) && (c & 1);
	zabs(tb, b);

	if (c & 1)
		zset(a, tb);
	else
		zsetu(a, 1);
	while (c >>= 1) {
		zsqr_ll(tb, tb);
		if (c & 1)
			zmul_ll(a, a, tb);
	}

	if (neg)
		zneg(a, a);
}
