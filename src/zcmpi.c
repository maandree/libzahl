/* See LICENSE file for copyright and license details. */
#include "internals.h"


int
zcmpi(z_t a, int64_t b)
{
	if (unlikely(!b))
		return zsignum(a);
	if (unlikely(zzero(a)))
		return b > 0 ? -1 : b < 0;
	zseti(libzahl_tmp_cmp, b);
	return zcmp(a, libzahl_tmp_cmp);
}
