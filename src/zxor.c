/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zxor(z_t a, z_t b, z_t c)
{
	size_t n, m;

	if (EXPECT(zzero(b), 0)) {
		if (zzero(c))
			SET_SIGNUM(a, 0);
		else
			SET(a, c);
		return;
	} else if (EXPECT(zzero(c), 0)) {
		SET(a, b);
		return;
	}

	m = MAX(b->used, c->used);
	n = b->used + c->used - m;

	ENSURE_SIZE(a, m);

	if (a == b) {
		if (b->used < c->used)
			zmemcpy(a->chars + n, c->chars + n, m - n);
		while (n--)
			a->chars[n] ^= c->chars[n];
	} else if (EXPECT(a == c, 0)) {
		if (c->used < b->used)
			zmemcpy(a->chars + n, b->chars + n, m - n);
		while (n--)
			a->chars[n] ^= b->chars[n];
	} else if (m == b->used) {
		zmemcpy(a->chars, b->chars, m);
		while (n--)
			a->chars[n] ^= c->chars[n];
	} else {
		zmemcpy(a->chars, c->chars, m);
		while (n--)
			a->chars[n] ^= b->chars[n];
	}

	a->used = m;
	TRIM(a);
	if (a->used)
		SET_SIGNUM(a, 1 - 2 * ((zsignum(b) ^ zsignum(c)) < 0));
	else
		SET_SIGNUM(a, 0);
}
