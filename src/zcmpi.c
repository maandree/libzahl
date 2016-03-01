/* See LICENSE file for copyright and license details. */
#include "internals"


int
zcmpi(z_t a, long long int b)
{
	if (!b)
		return zsignum(a);
	if (zzero(a))
		return b > 0 ? -1 : b < 0;
	zseti(zahl_tmp_cmp, b);
	return zcmp(a, zahl_tmp_cmp);
}
