/* See LICENSE file for copyright and license details. */
#include "internals"


int
zbtest(z_t a, size_t bit)
{
	size_t chars;
	if (zzero(a))
		return 0;

	chars = bit >> LB_BITS_PER_CHAR;
	if (chars >= a->used)
		return 0;

	bit &= BITS_PER_CHAR - 1;
	return (a->chars[chars] >> bit) & 1;
}
