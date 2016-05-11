/* See LICENSE file for copyright and license details. */
#include "internals.h"

#define ta          libzahl_tmp_divmod_a
#define tb          libzahl_tmp_divmod_b
#define td          libzahl_tmp_divmod_d
#define tds_proper  libzahl_tmp_divmod_ds


static inline void
zdivmod_impl(z_t a, z_t b, z_t c, z_t d)
{
	size_t c_bits, d_bits, bit, i;
	static z_t tds[BITS_PER_CHAR];

	c_bits = zbits(c);
	d_bits = zbits(d);

	bit = c_bits - d_bits;
	zlsh(td, d, bit);
	SET_SIGNUM(td, 1);
	if (zcmpmag(td, c) > 0) {
		zrsh(td, td, 1);
		bit -= 1;
	}

	SET_SIGNUM(ta, 0);
	zabs(tb, c);

	if (unlikely(bit <= BITS_PER_CHAR)) {
		for (;;) {
			if (zcmpmag(td, tb) <= 0) {
				zsub_unsigned(tb, tb, td);
				zbset(ta, ta, bit, 1);
			}
			if (!bit-- || zzero(tb))
				goto done;
			zrsh(td, td, 1);
		}
	} else {
		for (i = 0; i < BITS_PER_CHAR; i++) {
			zrsh(tds_proper[i], td, i);
			tds[i]->used = tds_proper[i]->used;
			tds[i]->sign = tds_proper[i]->sign;
			tds[i]->chars = tds_proper[i]->chars;
		}
		for (;;) {
			for (i = 0; i < BITS_PER_CHAR; i++) {
				if (zcmpmag(tds[i], tb) <= 0) {
					zsub_unsigned(tb, tb, tds[i]);
					zbset(ta, ta, bit, 1);
				}
				if (!bit-- || zzero(tb))
					goto done;
			}
			for (i = MIN(bit, BITS_PER_CHAR - 1) + 1; i--;)
				zrsh_taint(tds[i], BITS_PER_CHAR);
		}
	}
done:

	zswap(a, ta);
	zswap(b, tb);
}


void
zdivmod(z_t a, z_t b, z_t c, z_t d)
{
	int c_sign, sign, cmpmag;

	c_sign = zsignum(c);
	sign = c_sign * zsignum(d);

	if (unlikely(!sign)) {
		if (check(!zzero(c))) {
			libzahl_failure(-ZERROR_DIV_0);
		} else if (check(zzero(d))) {
			libzahl_failure(-ZERROR_0_DIV_0);
		} else {
			SET_SIGNUM(a, 0);
			SET_SIGNUM(b, 0);
		}
		return;
	} else if (cmpmag = zcmpmag(c, d), unlikely(cmpmag <= 0)) {
		if (unlikely(cmpmag == 0)) {
			zseti(a, sign);
			SET_SIGNUM(b, 0);
		} else {
			SET(b, c);
			SET_SIGNUM(a, 0);
		}
		return;
	}

	zdivmod_impl(a, b, c, d);
	SET_SIGNUM(a, sign);
	if (zsignum(b) > 0)
		SET_SIGNUM(b, c_sign);
}
