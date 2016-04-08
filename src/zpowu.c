/* See LICENSE file for copyright and license details. */
#include "internals.h"

#define tb  libzahl_tmp_pow_b


void
zpowu(z_t a, z_t b, unsigned long long int c)
{
	int neg;

	if (unlikely(!c)) {
		if (zzero(b))
			libzahl_failure(-ZERROR_0_POW_0);
		zsetu(a, 1);
		return;
	} else if (unlikely(zzero(b))) {
		SET_SIGNUM(a, 0);
		return;
	}

	neg = znegative(b) && (c & 1);
	zabs(tb, b);
	zsetu(a, 1);

	for (; c; c >>= 1) {
		if (c & 1)
			zmul_impl(a, a, tb);
		zsqr_impl(tb, tb);
	}

	if (neg)
		zneg(a, a);
}
