/* See LICENSE file for copyright and license details. */
#include "internals.h"

#include <stdio.h>

#define num  libzahl_tmp_str_num
#define rem  libzahl_tmp_str_rem

/* All 19 you see here is derived from that 10¹⁹ is the largest
 * power of than < 2⁶⁴, and 64 is the number of bits in
 * zahl_char_t. If zahl_char_t is chanced, the value 19, and
 * the cast to unsigned long long must be changed accordingly. */


char *
zstr(z_t a, char *b)
{
	char buf[19 + 1];
	size_t n, len, neg;
	char overridden = 0;

	if (unlikely(zzero(a))) {
		if (unlikely(!b) && unlikely(!(b = malloc(2))))
			libzahl_failure(errno);
		b[0] = '0';
		b[1] = 0;
		return b;
	}

	n = zstr_length(a, 10);

	if (unlikely(!b) && unlikely(!(b = malloc(n + 1))))
		libzahl_failure(errno);

	neg = znegative(a);
	zabs(num, a);
	b[0] = '-';
	b += neg;
	n -= neg;
	n = n > 19 ? (n - 19) : 0;

	for (;;) {
		zdivmod(num, rem, num, libzahl_const_1e19);
		if (likely(!zzero(num))) {
			sprintf(b + n, "%019llu", zzero(rem) ? 0ULL : (unsigned long long)(rem->chars[0]));
			b[n + 19] = overridden;
			overridden = b[n];
			n = n > 19 ? (n - 19) : 0;
		} else {
			len = (size_t)sprintf(buf, "%llu", (unsigned long long)(rem->chars[0]));
			if (overridden)
				buf[len] = b[n + len];
			memcpy(b + n, buf, len + 1);
			break;
		}
	}

	return b - neg;
}
