/* See LICENSE file for copyright and license details. */
#include "internals.h"


#if defined(__x86_64__)
# define ASM3(code)  \
	__asm__ __volatile__ (code : "+d"(carry) : "a"(ac + i), "b"(bc + i), "c"(cc + i))

# define ASM2(code)  \
	__asm__ __volatile__ (code : "+d"(carry) : "a"(ac + i), "b"(bc + i))

# define ADD2(off)                         \
	"\n    movq "#off"(%%rbx), %%rdx"  \
	"\n    adcq %%rdx, "#off"(%%rax)"

# define ADD3(off)                         \
	"\n    movq "#off"(%%rbx), %%rdx"  \
	"\n    adcq "#off"(%%rcx), %%rdx"  \
	"\n    movq %%rdx, "#off"(%%rax)"

# define WRAP_CARRY(interior)   \
	"\n    clc"             \
	"\n    cmpq $0, %%rdx"  \
	"\n    je 1f"           \
	"\n    stc"             \
	"\n 1:"                 \
	interior                \
	"\n    movq $1, %%rdx"  \
	"\n    jc 1f"           \
	"\n    movq $0, %%rdx"  \
	"\n 1:"
#endif


static inline void
zadd_impl_4(z_t a, z_t b, z_t c, size_t n)
{
	zahl_char_t carry = 0, *ac = a->chars, *bc = b->chars, *cc = c->chars;
	size_t i;

#if defined(__x86_64__)
	for (i = 0; (i += 4) <= n;)
		ASM3(WRAP_CARRY(ADD3(-32) ADD3(-24) ADD3(-16) ADD3(-8)));
	if (i > n) {
		i -= 4;
		switch (n & 3) {
		case 3:
			ASM3(WRAP_CARRY(ADD3(0) ADD3(8) ADD3(16)));
			break;
		case 2:
			ASM3(WRAP_CARRY(ADD3(0) ADD3(8)));
			break;
		case 1:
			ASM3(WRAP_CARRY(ADD3(0)));
			break;
		default:
			break;
		}
	}
	i = n;

	while (carry) {
		carry = libzahl_add_overflow(ac + i, ac[i], 1);
		i++;
	}
#else
	zahl_char_t tcarry;

	for (i = 0; i < n; i++) {
		tcarry = libzahl_add_overflow(ac + i, bc[i], cc[i]);
		carry = tcarry | (zahl_char_t)libzahl_add_overflow(ac + i, ac[i], carry);
	}
	while (carry) {
		carry = libzahl_add_overflow(ac + i, ac[i], 1);
		i++;
	}
#endif

	if (a->used < i)
		a->used = i;
}

static inline void
zadd_impl_3(z_t a, z_t b, size_t n)
{
#if defined(__x86_64__)
	zahl_char_t carry = 0, *ac = a->chars, *bc = b->chars;
	size_t i;

	for (i = 0; (i += 4) <= n;)
		ASM2(WRAP_CARRY(ADD2(-32) ADD2(-24) ADD2(-16) ADD2(-8)));
	if (i > n) {
		i -= 4;
		switch (n & 3) {
		case 3:
			ASM2(WRAP_CARRY(ADD2(0) ADD2(8) ADD2(16)));
			break;
		case 2:
			ASM2(WRAP_CARRY(ADD2(0) ADD2(8)));
			break;
		case 1:
			ASM2(WRAP_CARRY(ADD2(0)));
			break;
		default:
			break;
		}
	}
	i = n;

	while (carry) {
		carry = libzahl_add_overflow(ac + i, ac[i], 1);
		i++;
	}

	if (a->used < i)
		a->used = i;
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
