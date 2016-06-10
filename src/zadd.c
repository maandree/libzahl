/* See LICENSE file for copyright and license details. */
#include "internals.h"


#if defined(__x86_64__) && !defined(ZAHL_NO_ASM)
# define ASM3(code)  \
	__asm__ __volatile__ (code : [x]"+r"(carry), [a]"+r"(ac), [b]"+r"(bc), [c]"+r"(cc))

# define ASM2(code)  \
	__asm__ __volatile__ (code : [x]"+r"(carry), [a]"+r"(ac), [b]"+r"(bc))

# define ADD2(off)                       \
	"\n    movq "#off"(%[b]), %[x]"  \
	"\n    adcq %[x], "#off"(%[a])"

# define ADD3(off)                       \
	"\n    movq "#off"(%[b]), %[x]"  \
	"\n    adcq "#off"(%[c]), %[x]"  \
	"\n    movq %[x], "#off"(%[a])"

# define WRAP_CARRY(interior)   \
	"\n    addq $-1, %[x]"  \
	interior                \
	"\n    movq $1, %[x]"   \
	"\n    jc 1f"           \
	"\n    movq $0, %[x]"   \
	"\n 1:"
/*
 * I have already tried setc, cmovnc, cmovc, and adc,
 * instead of the last four lines. There does not seem
 * to be any better why to store the carry flag.
 */

# define ASM_ADD(N)                                                                          \
	do {                                                                                 \
		register zahl_char_t carry = 0;                                              \
		size_t i;                                                                    \
		for (i = 0; (INC(4)), (i += 4) <= n;)                                        \
			ASM##N(WRAP_CARRY(ADD##N(-32) ADD##N(-24) ADD##N(-16) ADD##N(-8)));  \
		switch (n & 3) {                                                             \
		case 3:                                                                      \
			ASM##N(WRAP_CARRY(ADD##N(-32) ADD##N(-24) ADD##N(-16)));             \
			break;                                                               \
		case 2:                                                                      \
			ASM##N(WRAP_CARRY(ADD##N(-32) ADD##N(-24)));                         \
			break;                                                               \
		case 1:                                                                      \
			ASM##N(WRAP_CARRY(ADD##N(-32)));                                     \
			break;                                                               \
		default:                                                                     \
			break;                                                               \
		}                                                                            \
		i = n;                                                                       \
		while (carry) {                                                              \
			carry = libzahl_add_overflow(a->chars + i, a->chars[i], 1);          \
			i++;                                                                 \
		}                                                                            \
		if (a->used < i)                                                             \
			a->used = i;                                                         \
	} while (0)
#endif


static inline void
zadd_impl_4(z_t a, z_t b, z_t c, size_t n)
{
#ifdef ASM_ADD
	register zahl_char_t *ac = a->chars, *bc = b->chars, *cc = c->chars;
# define INC(P)  (ac += (P), bc += (P), cc += (P))
	ASM_ADD(3);
# undef INC
#else
	zahl_char_t carry = 0, tcarry;
	zahl_char_t *ac = a->chars, *bc = b->chars, *cc = c->chars;
	size_t i;

	for (i = 0; i < n; i++) {
		tcarry = libzahl_add_overflow(ac + i, bc[i], cc[i]);
		carry = tcarry | (zahl_char_t)libzahl_add_overflow(ac + i, ac[i], carry);
	}

	while (carry) {
		carry = libzahl_add_overflow(ac + i, ac[i], 1);
		i++;
	}

	if (a->used < i)
		a->used = i;
#endif
}

static inline void
zadd_impl_3(z_t a, z_t b, size_t n)
{
#ifdef ASM_ADD
	register zahl_char_t *ac = a->chars, *bc = b->chars;
# define INC(P)  (ac += (P), bc += (P))
	ASM_ADD(2);
# undef INC
#else
	zadd_impl_4(a, a, b, n);
#endif
}

static inline void
libzahl_zadd_unsigned(z_t a, z_t b, z_t c)
{
	size_t size, n;

	if (unlikely(zzero(b))) {
		zabs(a, c);
		return;
	} else if (unlikely(zzero(c))) {
		zabs(a, b);
		return;
	}

	size = MAX(b->used, c->used);
	n = b->used + c->used - size;

	ENSURE_SIZE(a, size + 1);
	a->chars[size] = 0;

	if (a == b) {
		if (a->used < c->used) {
			n = c->used;
			zmemset(a->chars + a->used, 0, n - a->used);
		}
		zadd_impl_3(a, c, n);
	} else if (unlikely(a == c)) {
		if (a->used < b->used) {
			n = b->used;
			zmemset(a->chars + a->used, 0, n - a->used);
		}
		zadd_impl_3(a, b, n);
	} else if (likely(b->used > c->used)) {
		zmemcpy(a->chars + n, b->chars + n, size - n);
		a->used = size;
		zadd_impl_4(a, b, c, n);
	} else {
		zmemcpy(a->chars + n, c->chars + n, size - n);
		a->used = size;
		zadd_impl_4(a, b, c, n);
	}

	SET_SIGNUM(a, 1);
}

void
zadd_unsigned(z_t a, z_t b, z_t c)
{
	libzahl_zadd_unsigned(a, b, c);
}

void
zadd_unsigned_assign(z_t a, z_t b)
{
	size_t size, n;

	if (unlikely(zzero(a))) {
		zabs(a, b);
		return;
	} else if (unlikely(zzero(b))) {
		return;
	}

	size = MAX(a->used, b->used);
	n = a->used + b->used - size;

	ENSURE_SIZE(a, size + 1);
	a->chars[size] = 0;

	if (a->used < b->used) {
		n = b->used;
		zmemset(a->chars + a->used, 0, n - a->used);
	}
	zadd_impl_3(a, b, n);

	SET_SIGNUM(a, 1);
}

void
zadd(z_t a, z_t b, z_t c)
{
	if (unlikely(zzero(b))) {
		SET(a, c);
	} else if (unlikely(zzero(c))) {
		SET(a, b);
	} else if (unlikely(znegative(b))) {
		if (znegative(c)) {
			libzahl_zadd_unsigned(a, b, c);
			SET_SIGNUM(a, -zsignum(a));
		} else {
			zsub_unsigned(a, c, b);
		}
	} else if (unlikely(znegative(c))) {
		zsub_unsigned(a, b, c);
	} else {
		libzahl_zadd_unsigned(a, b, c);
	}
}
