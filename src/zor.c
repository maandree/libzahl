/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zor(z_t a, z_t b, z_t c)
{
	size_t n, m;

	if (zzero(b)) {
		if (zzero(c))
			SET_SIGNUM(a, 0);
		else
			SET(a, c);
		return;
	} else if (zzero(c)) {
		SET(a, b);
		return;
	}

	m = MAX(b->used, c->used);
	n = b->used + c->used - m;
	a->used = m;

	ENSURE_SIZE(a, m);

	if (a == b) {
		zmemcpy(a->chars + n, m == b->used ? b->chars : c->chars, m - n);
		while (n--)
			a->chars[n] |= c->chars[n];
	} else if (a == c) {
		zmemcpy(a->chars + n, m == b->used ? b->chars : c->chars, m - n);
		while (n--)
			a->chars[n] |= b->chars[n];
	} else if (m == b->used) {
		zmemcpy(a->chars, b->chars, m);
		while (n--)
			a->chars[n] |= c->chars[n];
	} else {
		zmemcpy(a->chars, c->chars, m);
		while (n--)
			a->chars[n] |= b->chars[n];
	}

	SET_SIGNUM(a, (zsignum(b) > 0 && zsignum(c) > 0) * 2 - 1);
}
