/* See LICENSE file for copyright and license details. */
#include "internals.h"

#define u  libzahl_tmp_gcd_u
#define v  libzahl_tmp_gcd_v


void
zgcd(z_t a, z_t b, z_t c)
{
	/*
	 * Binary GCD algorithm.
	 */

	size_t shifts;
	zahl_char_t *u_orig, *v_orig;
	size_t u_lsb, v_lsb;
	int neg, cmpmag;

	if (unlikely(zzero(b))) {
		SET(a, c);
		return;
	}
	if (unlikely(zzero(c))) {
		SET(a, b);
		return;
	}

	neg = znegative2(b, c);

	u_lsb = zlsb(b);
	v_lsb = zlsb(c);
	shifts = MIN(u_lsb, v_lsb);
	zrsh(u, b, u_lsb);
	zrsh(v, c, v_lsb);

	u_orig = u->chars;
	v_orig = v->chars;

	for (;;) {
		if (likely((cmpmag = zcmpmag(u, v)) >= 0)) {
			if (unlikely(cmpmag == 0))
				break;
			zswap_tainted_unsigned(u, v);
		}
		zsub_positive_assign(v, u);
		zrsh_taint(v, zlsb(v));
	}

	zlsh(a, u, shifts);
	SET_SIGNUM(a, neg ? -1 : 1);

	u->chars = u_orig;
	v->chars = v_orig;
}
