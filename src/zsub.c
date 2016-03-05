/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zsub_unsigned(z_t a, z_t b, z_t c)
{
	zahl_char_t carry[] = {0, 0};
	zahl_char_t *s;
	size_t i, n;
	int magcmp;

	if (zzero(b)) {
		zabs(a, c);
		zneg(a, a);
		return;
	} else if (zzero(c)) {
		zabs(a, b);
		return;
	} else if (a == b || a == c) {
		/* TODO This should not be necessary. */
		z_t tb, tc;
		zinit(tb);
		zinit(tc);
		zset(tb, b);
		zset(tc, c);
		zsub_unsigned(a, tb, tc);
		zfree(tb);
		zfree(tc);
		return;
	}

	magcmp = zcmpmag(b, c);
	if (magcmp <= 0) {
		if (magcmp == 0) {
			SET_SIGNUM(a, 0);
			return;
		}
		SET(a, c);
		n = MIN(a->used, b->used);
		s = b->chars;
	} else {
		SET(a, b);
		n = MIN(a->used, c->used);
		s = c->chars;
	}

	for (i = 0; i < n; i++) {
		carry[~i & 1] = carry[i & 1] ? (a->chars[i] <= s[i]) : (a->chars[i] < s[i]);
		a->chars[i] -= s[i];
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
