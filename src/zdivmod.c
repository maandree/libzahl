/* See LICENSE file for copyright and license details. */
#include "internals"

#define ta   libzahl_tmp_divmod_a
#define tb   libzahl_tmp_divmod_b
#define td   libzahl_tmp_divmod_d
#define tds  libzahl_tmp_divmod_ds


void
zdivmod(z_t a, z_t b, z_t c, z_t d)
{
	size_t c_bits, d_bits, bit;
	int sign, cmpmag;

	sign = zsignum(c) * zsignum(d);

	if (!sign) {
		if (zzero(c)) {
			if (zzero(d)) {
				errno = EDOM; /* Indeterminate form: 0 divided by 0 */
				FAILURE_JUMP();
			} else {
				SET_SIGNUM(a, 0);
				SET_SIGNUM(b, 0);
			}
		} else {
			errno = EDOM; /* Undefined form: Division by 0 */
			FAILURE_JUMP();
		}
		return;
	} else if ((cmpmag = zcmpmag(c, d)) <= 0) {
	  	if (cmpmag == 0) {
			zseti(a, sign);
			SET_SIGNUM(b, 0);
			return;
		} else if (sign < 0) {
			zsub_unsigned(b, d, c);
		} else if (b != c) {
			zset(b, c);
		}
		SET_SIGNUM(b, 1);
		SET_SIGNUM(a, 0);
		return;
	}

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

	if (bit < BITS_PER_CHAR) {
		for (;;) {
			if (zcmpmag(td, tb) <= 0) {
				zsub(tb, tb, td);
				zbset(ta, ta, bit, 1);
			}
			if (!bit--)
				goto done;
			zrsh(td, td, 1);
		}
	} else {
		size_t i;
		for (i = 0; i < BITS_PER_CHAR; i++)
			zrsh(tds[i], td, i);
		for (;;) {
			for (i = 0; i < BITS_PER_CHAR; i++) {
				if (zcmpmag(td[i], tb) <= 0) {
					zsub(tb, tb, td[i]);
					zbset(ta, ta, bit, 1);
				}
				if (!bit--)
					goto done;
				zrsh(td[i], td[i], 1);
			}
			for (i = bit < BITS_PER_CHAR ? bit : BITS_PER_CHAR; i--;)
				zrsh(tds[i], tds[i], BITS_PER_CHAR);
		}
	}
done:

	zset(a, ta);
	zset(b, tb);
	SET_SIGNUM(a, sign);
}
