/* See LICENSE file for copyright and license details. */
#include "internals"


void
zswap(z_t a, z_t b)
{
	z_t t;
	*t = *a;
	*a = *b;
	*b = t;
}
