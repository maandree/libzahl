/* See LICENSE file for copyright and license details. */
#include "internals.h"


int
zcmpu(z_t a, unsigned long long int b)
{
	if (unlikely(!b))
		return zsignum(a);
	if (unlikely(zsignum(a) <= 0))
		return -1;
	zsetu(libzahl_tmp_cmp, b);
	return zcmp(a, libzahl_tmp_cmp);
}
