/* See LICENSE file for copyright and license details. */
#include "internals.h"

#define tb  libzahl_tmp_pow_b
#define td  libzahl_tmp_pow_d


void
zmodpowu(z_t a, z_t b, unsigned long long int c, z_t d)
{
	if (unlikely(!c)) {
		if (check(zzero(b)))
			libzahl_failure(-ZERROR_0_POW_0);
		else if (check(zzero(d)))
			libzahl_failure(-ZERROR_DIV_0);
		else
			zsetu(a, 1);
		return;
	} else if (check(zzero(d))) {
		libzahl_failure(-ZERROR_DIV_0);
	} else if (unlikely(zzero(b))) {
		SET_SIGNUM(a, 0);
		return;
	}

	zmod(tb, b, d);
	zset(td, d);

	if (c & 1)
		zset(a, tb);
	else
		zsetu(a, 1);
	while (c >>= 1) {
		zmodsqr(tb, tb, td);
		if (c & 1)
			zmodmul(a, a, tb, td);
	}
}
