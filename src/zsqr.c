/* See LICENSE file for copyright and license details. */
#include "internals.h"


static inline void
zsqr_ll_single_char(z_t a, z_t b)
{
	ENSURE_SIZE(a, 1);
	a->used = 1;
	a->chars[0] = b->chars[0] * b->chars[0];
	SET_SIGNUM(a, 1);
}

void
zsqr_ll(z_t a, z_t b)
{
	/*
	 * Karatsuba algorithm, optimised for equal factors.
	 */

#define z2 a
	z_t z0, z1, high, low;
	zahl_char_t auxchars[3 * ZAHL_FLUFF];
	size_t bits;

	bits = zbits(b);

	if (bits <= BITS_PER_CHAR / 2) {
		zsqr_ll_single_char(a, b);
		return;
	}

	bits >>= 1;

	/* Try to split only at a character level rather than a bit level.
	 * Such splits are faster, even if bit-level is required, and do
	 * not require auxiliary memory except for the bit-level split
	 * which require constant auxiliary memory. */
	if (bits < BITS_PER_CHAR) {
		low->chars = auxchars;
		high->chars = auxchars + ZAHL_FLUFF;
		zsplit_unsigned_fast_small_auto(high, low, b, bits);
	} else {
		bits = TRUNCATE_TO_CHAR(bits);
		zsplit_unsigned_fast_large_taint(high, low, b, bits);
	}


	if (unlikely(zzero(low))) {
		zsqr_ll(z2, high);
		zlsh(a, z2, bits << 1);
	} else {
		zinit_temp(z0);
		zinit_temp(z1);

		zsqr_ll(z0, low);

		zmul_ll(z1, low, high);
		zlsh(z1, z1, bits + 1);

		zsqr_ll(z2, high);
		zlsh(a, z2, bits << 1);

		zadd_unsigned_assign(a, z1);
		zadd_unsigned_assign(a, z0);

		zfree_temp(z1);
		zfree_temp(z0);
	}
}
