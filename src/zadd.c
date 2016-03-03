/* See LICENSE file for copyright and license details. */
#include "internals"

#include <stdlib.h>
#include <string.h>


void
zadd_unsigned(z_t a, z_t b, z_t c)
{
	size_t i, size, n;
	uint32_t carry[] = {0, 0};
	zahl_char_t *addend;

	if (a == c) {
		zadd_unsigned(a, c, b);
		return;
	}

	size = b->used > c->used ? b->used : c->used;
	if (a->alloced < size + 1) {
		a->alloced = size + 1;
		a->chars = realloc(a->chars, a->alloced * sizeof(*(a->chars)));
	}
	a->chars[size] = 0;

	n = b->used + c->used - size;
	if (a == b) {
		if (a->used < c->used) {
			n = c->used;
			memset(a->chars + a->used, 0, (n - a->used) * sizeof(*(a->chars)));
		}
		addend = c->chars;
	} else if (a == c) {
		if (a->used < b->used) {
			n = b->used;
			memset(a->chars + a->used, 0, (n - a->used) * sizeof(*(a->chars)));
		}
		addend = b->chars;
	} else if (b->used > c->used) {
		memcpy(a->chars, b->chars, b->used * sizeof(*(a->chars)));
		a->used = b->used;
		addend = c->chars;
	} else {
		memcpy(a->chars, c->chars, c->used * sizeof(*(a->chars)));
		a->used = c->used;
		addend = b->chars;
	}

	for (i = 0; i < n; i++) {
		if (carry[i & 1])
			carry[~i & 1] = (ZAHL_CHAR_MAX - a->chars[i] <= addend[i]);
		else
			carry[~i & 1] = (ZAHL_CHAR_MAX - a->chars[i] < addend[i]);
		a->chars[i] += addend[i] + carry[i & 1];
	}

	while (carry[~i & 1]) {
		carry[i & 1] = a->chars[i] == ZAHL_CHAR_MAX;
		a->chars[i++] += 1;
	}

	if (a->used < i)
		a->used = i;
	SET_SIGNUM(a, !zzero(b) | !zzero(c));
}


void
zadd(z_t a, z_t b, z_t c)
{
	if (zzero(b)) {
		if (a != b)
			zset(a, c);
	} else if (zzero(c)) {
		if (a != b)
			zset(a, b);
	} else if (b == c) {
		zlsh(a, b, 1);
	} else if ((zsignum(b) | zsignum(c)) < 0) {
		if (zsignum(b) < 0) {
			if (zsignum(c) < 0) {
				zadd_unsigned(a, b, c);
				SET_SIGNUM(a, -zsignum(a));
			} else {
				zsub_unsigned(a, c, b);
			}
		} else {
			zsub_unsigned(a, b, c);
		}
	} else {
		zadd_unsigned(a, b, c);
	}
}
