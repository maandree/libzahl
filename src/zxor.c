/* See LICENSE file for copyright and license details. */
#include "internals"

#include <string.h>
#include <stdlib.h>


void
zxor(z_t a, z_t b, z_t c)
{
	size_t n, m;

	if (zzero(b)) {
		if (zzero(c)) {
			SET_SIGNUM(a, 0);
		} else {
			if (a != c)
				zset(a, c);
		}
		return;
	} else if (zzero(c)) {
		if (a != b)
			zset(a, b);
		return;
	}

	m = b->used > c->used ? b->used : c->used;
	n = b->used + c->used - m;

	if (n == m && !memcmp(b->chars, c->chars, m * sizeof(*(b->chars)))) {
		SET_SIGNUM(a, 0);
		return;
	}

	if (a->alloced < m) {
		a->alloced = m;
		a->chars = realloc(a->chars, m * sizeof(*(a->chars)));
	}

	if (a == b) {
		memcpy(a->chars + n, m == b->used ? b->chars : c->chars, (m - n) * sizeof(*(a->chars)));
		while (n--)
			a->chars[n] ^= c->chars[n];
	} else if (a == c) {
		memcpy(a->chars + n, m == b->used ? b->chars : c->chars, (m - n) * sizeof(*(a->chars)));
		while (n--)
			a->chars[n] ^= b->chars[n];
	} else if (m == b->used) {
		memcpy(a->chars, b->chars, m * sizeof(*(a->chars)));
		while (n--)
			a->chars[n] ^= c->chars[n];
	} else {
		memcpy(a->chars, c->chars, m * sizeof(*(a->chars)));
		while (n--)
			a->chars[n] ^= b->chars[n];
	}

	SET_SIGNUM(a, 1 - 2 * ((zsignum(b) ^ zsignum(c)) < 0);
}
