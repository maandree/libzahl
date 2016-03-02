/* See LICENSE file for copyright and license details. */
#include "internals"


void
zsplit(z_t high, z_t low, z_t a, size_t delim)
{
	size_t chars;

	if (zzero(a)) {
		SET_SIGNUM(high, 0);
		SET_SIGNUM(low, 0);
		return;
	}

	chars = delim >> LB_BITS_PER_CHAR;

	if (high == a) {
		if (a->used < chars)
			SET_SIGNUM(low, 0);
		else
			ztrunc(low, a, delim);
		zrsh(high, a, delim);
	} else {
		zrsh(high, a, delim);
		if (a->used < chars)
			SET_SIGNUM(low, 0);
		else
			ztrunc(low, a, delim);
	}
}
