/* See LICENSE file for copyright and license details. */
#include "internals.h"


static inline void
zsqr_impl_single_char(z_t a, z_t b)
{
	ENSURE_SIZE(a, 1);
	a->used = 1;
	a->chars[0] = b->chars[0] * b->chars[0];
	SET_SIGNUM(a, 1);
}

static void
zsqr_impl(z_t a, z_t b)
{
	/*
	 * Karatsuba algorithm, optimised for equal factors.
	 */

	z_t z0, z1, z2, high, low;
	size_t bits;
	zahl_char_t auxchars[3];

	bits = zbits(b);

	if (bits <= BITS_PER_CHAR / 2) {
		zsqr_impl_single_char(a, b);
		return;
	}

	bits >>= 1;

	zinit(z0);
	zinit(z1);
	zinit(z2);

	if (bits < BITS_PER_CHAR) {
		low->chars = auxchars;
		high->chars = auxchars + 1;
		zsplit_fast_small_tainted(high, low, b, bits);
	} else {
		bits &= ~(BITS_PER_CHAR - 1);
		zsplit_fast_large_taint(high, low, b, bits);
	}


	zsqr_impl(z2, high);
	if (unlikely(zzero(low))) {
		SET_SIGNUM(z0, 0);
		SET_SIGNUM(z1, 0);
	} else {
		zsqr_impl(z0, low);
		zmul(z1, low, high);
	}

	zlsh(z1, z1, bits + 1);
	bits <<= 1;
	zlsh(a, z2, bits);
	zadd_unsigned_assign(a, z1);
	zadd_unsigned_assign(a, z0);


	zfree(z0);
	zfree(z1);
	zfree(z2);
}

void
zsqr(z_t a, z_t b)
{
	if (unlikely(zzero(b))) {
		SET_SIGNUM(a, 0);
	} else {
		zsqr_impl(a, b);
		SET_SIGNUM(a, 1);
	}
}
