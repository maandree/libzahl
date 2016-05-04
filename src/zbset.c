/* See LICENSE file for copyright and license details. */
#include "internals.h"


#define PROLOGUE(MAY_INCREASE)\
	zahl_char_t mask = 1;\
	size_t chars = FLOOR_BITS_TO_CHARS(bit);\
	if (MAY_INCREASE) {\
		if (zzero(a)) {\
			a->used = 0;\
			SET_SIGNUM(a, 1);\
		}\
		if (unlikely(chars >= a->used)) {\
			ENSURE_SIZE(a, chars + 1);\
			zmemset(a->chars + a->used, 0, chars + 1 - a->used);\
			a->used = chars + 1;\
		}\
	} else if (unlikely(chars >= a->used)) {\
		return;\
	}\
	bit = BITS_IN_LAST_CHAR(bit);\
	mask <<= bit


void
zbset_ll_set(z_t a, size_t bit)
{
	PROLOGUE(1);
	a->chars[chars] |= mask;
}

void
zbset_ll_clear(z_t a, size_t bit)
{
	PROLOGUE(0);
	a->chars[chars] &= ~mask;
	TRIM_AND_ZERO(a);
}

void
zbset_ll_flip(z_t a, size_t bit)
{
	PROLOGUE(1);
	a->chars[chars] ^= mask;
	TRIM_AND_ZERO(a);
}
