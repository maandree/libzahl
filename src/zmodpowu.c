/* See LICENSE file for copyright and license details. */
#include "internals"

#include <errno.h>

#define tb  libzahl_tmp_pow_b
#define td  libzahl_tmp_pow_d


void
zmodpowu(z_t a, z_t b, unsigned long long int c, z_t d)
{
	size_t i, n;

	if (!c) {
		if (zzero(b)) {
			errno = EDOM; /* Indeterminate form: 0:th power of 0 */
			FAILURE_JUMP();
		} else if (zzero(d)) {
			errno = EDOM; /* Undefined form: Division by 0 */
			FAILURE_JUMP();
		} else {
			zsetu(a, 1);
		}
		return;
	} else if (zzero(d)) {
		errno = EDOM; /* Undefined form: Division by 0 */
		FAILURE_JUMP();
	} else if (zzero(b)) {
		SET_SIGNUM(a, 0);
		return;
	}

	n = zbits(c);

	zmod(tb, b, d);
	zset(td, d);
	zsetu(a, 1);

	for (; c; c >>= 1) {
		if (c & 1)
			zmodmul(a, a, tb, td);
		zmodsqr(tb, tb, td);
	}
}
