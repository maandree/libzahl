/* See LICENSE file for copyright and license details. */
#include "internals.h"


static inline void
zmul_impl_single_char(z_t a, z_t b, z_t c)
{
	ENSURE_SIZE(a, 1);
	a->used = 1;
	a->chars[0] = b->chars[0] * c->chars[0];
	SET_SIGNUM(a, 1);
}

void
zmul_impl(z_t a, z_t b, z_t c)
{
	/*
	 * Karatsuba algorithm
	 * 
	 * Basically, this is how you were taught to multiply large numbers
	 * by hand in school: 4010⋅3020 = (4000 + 10)(3000 + 20) =
	 * = 40⋅30⋅10⁴ + (40⋅20 + 30⋅10)⋅10² + 10⋅20, but the middle is
	 * optimised to only one multiplication:
	 * 40⋅20 + 30⋅10 = (40 + 10)(30 + 20) − 40⋅30 − 10⋅20.
	 * This optimisation is crucial. Without it, the algorithm with
	 * run in O(n²).
	 */

#define z2 c_low
#define z1 b_low
#define z0 a
	size_t m, m2;
	z_t b_high, b_low, c_high, c_low;

	if (unlikely(zzero1(b, c))) {
		SET_SIGNUM(a, 0);
		return;
	}

	m = zbits(b);
	m2 = b == c ? m : zbits(c);

	if (m + m2 <= BITS_PER_CHAR) {
		zmul_impl_single_char(a, b, c);
		return;
	}

        m = MAX(m, m2);
	m2 = m >> 1;

	zinit(b_high);
	zinit(b_low);
	zinit(c_high);
	zinit(c_low);

	zsplit(b_high, b_low, b, m2);
	zsplit(c_high, c_low, c, m2);


	zmul_impl(z0, b_low, c_low);
	zadd_unsigned_assign(b_low, b_high);
	zadd_unsigned_assign(c_low, c_high);
	zmul_impl(z1, b_low, c_low);
	zmul_impl(z2, b_high, c_high);

	zsub_nonnegative_assign(z1, z0);
	zsub_nonnegative_assign(z1, z2);

	zlsh(z1, z1, m2);
	m2 <<= 1;
	zlsh(z2, z2, m2);
	zadd_unsigned_assign(a, z1);
	zadd_unsigned_assign(a, z2);


	zfree(b_high);
	zfree(b_low);
	zfree(c_high);
	zfree(c_low);
}

void
zmul(z_t a, z_t b, z_t c)
{
	int b_sign, c_sign;
	b_sign = b->sign, b->sign *= b_sign;
	c_sign = c->sign, c->sign *= c_sign;
	zmul_impl(a, b, c);
	c->sign *= c_sign;
	b->sign *= b_sign;
	SET_SIGNUM(a, zsignum(b) * zsignum(c));
}
