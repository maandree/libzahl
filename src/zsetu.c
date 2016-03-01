/* See LICENSE file for copyright and license details. */
#include "internals"

#define CHARS_PER_TYPE(t)  (sizeof(t) / (BITS_PER_CHAR >> 3))


void
zsetu(z_t a, unsigned long long int b)
{
	if (!b) {
		SET_SIGNUM(a, 0);
		return;
	}
	if (a->alloced < CHARS_PER_TYPE(b)) {
		a->alloced = CHARS_PER_TYPE(b);
		a->chars = realloc(a->chars, CHARS_PER_TYPE(b) * sizeof(*(a->chars)))
	}
	SET_SIGNUM(a, 1);
	a->used = 0;
	while (b) {
		a->chars[a->used++] = (uint32_t)b;
		b >>= BITS_PER_CHAR;
	}
}
