/* See LICENSE file for copyright and license details. */
#include "internals.h"

#include <stdio.h>

#define num  libzahl_tmp_str_num
#define rem  libzahl_tmp_str_rem

/* All 19 you see here is derived from that 10¹⁹ is the largest
 * power of than < 2⁶⁴, and 64 is the number of bits in
 * zahl_char_t. If zahl_char_t is chanced, the value 19, and
 * the cast to unsigned long long must be changed accordingly. */


#define S1(P)     P"0"    P"1"    P"2"    P"3"    P"4"    P"5"    P"6"    P"7"    P"8"    P"9"
#define S2(P)  S1(P"0")S1(P"1")S1(P"2")S1(P"3")S1(P"4")S1(P"5")S1(P"6")S1(P"7")S1(P"8")S1(P"9")


static inline O2 void
sprintint_fix(char *buf, zahl_char_t v)
{
	const char *partials = S2("");
	uint16_t *buffer = (uint16_t *)(buf + 1);

	buffer[8] = *(uint16_t *)(partials + 2 * (v % 100)), v /= 100;
	buffer[7] = *(uint16_t *)(partials + 2 * (v % 100)), v /= 100;
	buffer[6] = *(uint16_t *)(partials + 2 * (v % 100)), v /= 100;
	buffer[5] = *(uint16_t *)(partials + 2 * (v % 100)), v /= 100;
	buffer[4] = *(uint16_t *)(partials + 2 * (v % 100)), v /= 100;
	buffer[3] = *(uint16_t *)(partials + 2 * (v % 100)), v /= 100;
	buffer[2] = *(uint16_t *)(partials + 2 * (v % 100)), v /= 100;
	buffer[1] = *(uint16_t *)(partials + 2 * (v % 100)), v /= 100;
	buffer[0] = *(uint16_t *)(partials + 2 * (v % 100)), v /= 100;
	*buf = '0' + v;
	buf[19] = 0;
}

static inline void
cmemmove(char *d, const char *s, long n)
{
	while (n--)
		*d++ = *s++;
}

static inline size_t
sprintint_min(char *buf, zahl_char_t v)
{
	long i = 0, j;
	sprintint_fix(buf, v);
	for (; buf[i] == '0'; i++);
	cmemmove(buf, buf + i, j = 19 - i);
	buf[j] = 0;
	return j;
}


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
			sprintint_fix(b + n, zzero(rem) ? 0 : rem->chars[0]);
			b[n + 19] = overridden;
			overridden = b[n];
			n = n > 19 ? (n - 19) : 0;
		} else {
			len = sprintint_min(buf, rem->chars[0]);
			if (overridden)
				buf[len] = b[n + len];
			memcpy(b + n, buf, len + 1);
			break;
		}
	}

	return b - neg;
}
