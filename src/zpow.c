/* See LICENSE file for copyright and license details. */
#include "internals"

#include <errno.h>

#define tb  libzahl_tmp_pow_b
#define tc  libzahl_tmp_pow_c


void
zpow(z_t a, z_t b, z_t c)
{
	size_t i, n;

	if (zsignum(c) <= 0) {
		if (zzero(c)) {
			if (zzero(b)) {
				errno = EDOM; /* Indeterminate form: 0:th power of 0 */
				FAILURE_JUMP();
			}
			zsetu(a, 1);
		} else if (zzero(b)) {
			errno = EDOM; /* Undefined form: Division by 0 */
			FAILURE_JUMP();
		} else {
			SET_SIGNUM(a, 0);
		}
		return;
	} else if (zzero(b)) {
		SET_SIGNUM(a, 0);
		return;
	}

	n = zbits(c);

	zset(tb, b);
	zset(tc, c);
	zsetu(a, 1);

	for (i = 0; i < n; i++) {
		if (zbtest(tc, i))
			zmul(a, a, tb);
		zsqr(tb, tb);
	}
}
