/* See LICENSE file for copyright and license details. */
#include "internals.h"


int
zcmpi(z_t a, long long int b)
{
	if (EXPECT(!b, 0))
		return zsignum(a);
	if (EXPECT(zzero(a), 0))
		return b > 0 ? -1 : b < 0;
	zseti(libzahl_tmp_cmp, b);
	return zcmp(a, libzahl_tmp_cmp);
}
