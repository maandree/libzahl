/* See LICENSE file for copyright and license details. */
#include "internals"

#define ta  libzahl_tmp_divmod_a
#define tb  libzahl_tmp_divmod_b
#define td  libzahl_tmp_divmod_d
#define te  libzahl_tmp_divmod_e


void
zdivmod(z_t a, z_t b, z_t c, z_t d)
{
	size_t c_bits, d_bits, shift;
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

	shift = c_bits - d_bits;
	zlsh(td, d, shift);
	SET_SIGNUM(td, 1);
	if (zcmpmag(td, c) > 0) {
		zrsh(td, td, 1);
		shift -= 1;
	}

	zsetu(te, 1);
	zlsh(te, te, shift);
	SET_SIGNUM(ta, 0);
	zabs(tb, c);

	while (!zzero(te)) {
		if (zcmpmag(td, tb) <= 0) {
			zsub(tb, tb, td);
			zor(ta, ta, te);
		}
		zrsh(te, te, 1);
		zrsh(td, td, 1);
	}

	zset(a, ta);
	zset(b, tb);
	SET_SIGNUM(a, sign);
}
