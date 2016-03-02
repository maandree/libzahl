/* See LICENSE file for copyright and license details. */
#include "internals"

#define num  libzahl_tmp_str_num
#define rem  libzahl_tmp_str_rem

/* All 9 you see here is derived from that 10⁹ is the largest
 * power of than < 2³², and 32 is the number of bits in
 * zahl_char_t. If zahl_char_t is chanced, the value 9, and
 * the cast to unsigned long must be changed accordingly. */


char *
zstr(z_t a, char *b)
{
	size_t n;
	char overridden;
	int neg;

	if (zzero(a)) {
		if (!b) {
			b = malloc(2);
			if (!b)
				FAILURE_JUMP();
		}
		b[0] = '0';
		b[1] = 0;
		return b;
	}

	n = zstr_length(a, 10);
	if (!b) {
		b = malloc(n + 1);
		if (!b)
			FAILURE_JUMP();
	}

	neg = zsignum(a) < 0;
	zabs(num, a);
	n -= neg;
	n = n > 9 ? (n - 9) : 0;
	b[0] = '-';
	b += neg;
	overridden = 0;

	for (;;) {
		zdivmod(num, rem, num, libzahl_const_1e9);
		if (!zzero(num)) {
			sprintf(b + n, "%09lu", (unsigned long)(rem->chars[0]));
			b[n + 9] = overridden;
			overridden = b[n + (9 - 1)];
			n = n > 9 ? (n - 9) : 0;
		} else {
			n += sprintf(b + n, "%lu", (unsigned long)(rem->chars[0]));
			b[n] = overridden;
			break;
		}
	}

	return b - neg;
}
