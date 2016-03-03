/* See LICENSE file for copyright and license details. */
#include "internals"


void
zabs(z_t a, z_t b)
{
	if (a != b)
		zset(a, b);
	SET_SIGNUM(a, !zzero(a));
}
