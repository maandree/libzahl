/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zsub_unsigned(z_t a, z_t b, z_t c)
{
	int magcmp = zcmpmag(b, c);
	z_t s;
	size_t i, n;
	zahl_char_t carry[] = {0, 0};

	if (magcmp <= 0) {
		if (magcmp == 0) {
			SET_SIGNUM(a, 0);
			return;
		}
		SET(a, c);
		*s = *b;
	} else {
		SET(a, b);
		*s = *c;
	}

	n = MIN(a->used, s->used);

	for (i = 0; i < n; i++) {
		carry[~i & 1] = carry[i & 1] ? (a->chars[i] <= s->chars[i]) : (a->chars[i] < s->chars[i]);
		a->chars[i] -= s->chars[i];
		a->chars[i] -= carry[i & 1];
	}

	if (carry[i & 1]) {
		while (!a->chars[i])
			a->chars[i++] = ZAHL_CHAR_MAX;
		a->chars[i] -= 1;
	}
	SET_SIGNUM(a, magcmp);
}

void
zsub(z_t a, z_t b, z_t c)
{
	if (b == c) {
		SET_SIGNUM(a, 0);
	} else if (zzero(b)) {
		zneg(a, c);
	} else if (zzero(c)) {
		SET(a, b);
	} else if ((zsignum(b) | zsignum(c)) < 0) {
		if (zsignum(b) < 0) {
			if (zsignum(c) < 0) {
				zsub_unsigned(a, c, b);
			} else {
				zadd_unsigned(a, b, c);
				SET_SIGNUM(a, -zsignum(a));
			}
		} else {
			zadd_unsigned(a, b, c);
		}
	} else {
		zsub_unsigned(a, b, c);
	}
}
