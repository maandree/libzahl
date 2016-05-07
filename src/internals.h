/* See LICENSE file for copyright and license details. */
#include "../zahl.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* clang pretends to be GCC... */
#if defined(__GNUC__) && defined(__clang__)
# undef __GNUC__
#endif

#define BITS_PER_CHAR                ZAHL_BITS_PER_CHAR
#define LB_BITS_PER_CHAR             ZAHL_LB_BITS_PER_CHAR

#define FLOOR_BITS_TO_CHARS(bits)    ZAHL_FLOOR_BITS_TO_CHARS(bits)
#define CEILING_BITS_TO_CHARS(bits)  ZAHL_CEILING_BITS_TO_CHARS(bits)
#define BITS_IN_LAST_CHAR(bits)      ZAHL_BITS_IN_LAST_CHAR(bits)
#define TRUNCATE_TO_CHAR(bits)       ZAHL_TRUNCATE_TO_CHAR(bits)

#define O0     ZAHL_O0
#define O1     ZAHL_O1
#define O2     ZAHL_O2
#define O3     ZAHL_O3
#define Ofast  ZAHL_Ofast
#define Os     ZAHL_Os
#define Oz     ZAHL_Oz

#define LIST_TEMPS_HERE\
	X(libzahl_tmp_str_num, 0)\
	X(libzahl_tmp_str_mag, 0)\
	X(libzahl_tmp_str_div, 0)\
	X(libzahl_tmp_str_rem, 0)\
	X(libzahl_tmp_gcd_u, 0)\
	X(libzahl_tmp_gcd_v, 0)\
	X(libzahl_tmp_sub, 0)\
	X(libzahl_tmp_modmul, 0)\
	X(libzahl_tmp_pow_b, 0)\
	X(libzahl_tmp_pow_c, 0)\
	X(libzahl_tmp_pow_d, 0)\
	X(libzahl_tmp_modsqr, 0)\
	X(libzahl_tmp_divmod_a, 0)\
	X(libzahl_tmp_divmod_b, 0)\
	X(libzahl_tmp_divmod_d, 0)\
	X(libzahl_tmp_ptest_x, 0)\
	X(libzahl_tmp_ptest_a, 0)\
	X(libzahl_tmp_ptest_d, 0)\
	X(libzahl_tmp_ptest_n1, 0)\
	X(libzahl_tmp_ptest_n4, 0)

#define LIST_TEMPS\
	X(libzahl_tmp_div, 0)\
	X(libzahl_tmp_mod, 0)\
	LIST_TEMPS_HERE

#define LIST_CONSTS\
	X(0, libzahl_const_1e19, zsetu, 10000000000000000000ULL) /* The largest power of 10 < 2⁶⁴. */\
	X(1, libzahl_const_1,    zsetu, 1)\
	X(2, libzahl_const_2,    zsetu, 2)\
	X(3, libzahl_const_4,    zsetu, 4)

#define X(x, s)  extern z_t x;
LIST_TEMPS_HERE
#undef X
#define X(i, x, f, v)  extern z_t x;
LIST_CONSTS
#undef X

extern z_t libzahl_tmp_divmod_ds[BITS_PER_CHAR];
extern jmp_buf libzahl_jmp_buf;
extern int libzahl_set_up;
extern int libzahl_error;
extern zahl_char_t **libzahl_pool[sizeof(size_t) * 8];
extern size_t libzahl_pool_n[sizeof(size_t) * 8];
extern size_t libzahl_pool_alloc[sizeof(size_t) * 8];
extern struct zahl **libzahl_temp_stack;
extern struct zahl **libzahl_temp_stack_head;
extern struct zahl **libzahl_temp_stack_end;
extern void *libzahl_temp_allocation;

#define likely(expr)                 ZAHL_LIKELY(expr)
#define unlikely(expr)               ZAHL_UNLIKELY(expr)

#if defined(ZAHL_UNSAFE)
# define check(expr)                 0
#else
# define check(expr)                 unlikely(expr)
#endif

#define SET_SIGNUM(a, signum)        ZAHL_SET_SIGNUM(a, signum)
#define SET(a, b)                    ZAHL_SET(a, b)
#define ENSURE_SIZE(a, n)            ZAHL_ENSURE_SIZE(a, n)
#define TRIM(a)                      ZAHL_TRIM(a)
#define TRIM_NONZERO(a)              ZAHL_TRIM_NONZERO(a)
#define TRIM_AND_ZERO(a)             ZAHL_TRIM_AND_ZERO(a)
#define TRIM_AND_SIGN(a, s)          ZAHL_TRIM_AND_SIGN(a, s)
#define SWAP(a, b, t, m)             ((t)->m = (a)->m, (a)->m = (b)->m, (b)->m = (t)->m)
#define MIN(a, b)                    ((a) < (b) ? (a) : (b))
#define MAX(a, b)                    ((a) > (b) ? (a) : (b))
#define MIN_MAX_1(min, max, a, b)    ((min) = MIN(a, b), (max) = MAX(a, b))
#define MIN_MAX_2(min, max, a, b)    ((min) = (a) + (b) - ((max) = MAX(a, b)))
#define znegative(a)                 (zsignum(a) < 0)
#define znegative1(a, b)             ((zsignum(a) | zsignum(b)) < 0)
#define znegative2(a, b)             ((zsignum(a) & zsignum(b)) < 0)
#define zpositive(a)                 (zsignum(a) > 0)
#define zpositive1(a, b)             (zpositive(a) + zpositive(b) > 0)
#define zpositive2(a, b)             (zsignum(a) + zsignum(b) == 2)
#define zzero2(a, b)                 (!(zsignum(a) | zsignum(b)))
#define zmemcpy(d, s, n)             libzahl_memcpy(d, s, n)
#define zmemmove(d, s, n)            libzahl_memmove(d, s, n)
#define zmemmovef(d, s, n)           libzahl_memmovef(d, s, n)
#define zmemmoveb(d, s, n)           libzahl_memmoveb(d, s, n)
#define zmemset(a, v, n)             libzahl_memset(a, v, n)
#define zmemset_precise(a, v, n)     libzahl_memset_precise(a, v, n)

static inline int
zzero1(z_t a, z_t b)
{
	return zzero(a) || zzero(b);
}

static inline void
zmemcpy_range(register zahl_char_t *restrict d, register const zahl_char_t *restrict s, size_t i, size_t n)
{
	d += i;
	s += i;
	n -= i;
	zmemcpy(d, s, n);
}

static void
libzahl_failure(int error)
{
	libzahl_error = (error);
	if (libzahl_temp_stack)
		while (libzahl_temp_stack_head != libzahl_temp_stack)
			zfree(*--libzahl_temp_stack_head);
	free(libzahl_temp_allocation);
	libzahl_temp_allocation = 0;
	longjmp(libzahl_jmp_buf, 1);
}

static inline void
libzahl_memfailure(void)
{
	if (!errno) /* sigh... */
		errno = ENOENT;
	libzahl_failure(errno);
}

/*
 * libzahl_msb_nz_zu
 *         ^^^ ^^ ^^
 *         |   |  |
 *         |   |  \- size_t parameter
 *         |   \- non-zero input
 *         \- most significant bit
 */

#if SIZE_MAX == ULONG_MAX
# define libzahl_msb_nz_zu(x)        libzahl_msb_nz_lu(x)
#else
# define libzahl_msb_nz_zu(x)        libzahl_msb_nz_llu(x)
#endif

#if defined(__GNUC__) || defined(__clang__)
# define libzahl_msb_nz_lu(x)        (8 * sizeof(unsigned long int) - 1 - (size_t)__builtin_clzl(x))
# define libzahl_msb_nz_llu(x)       (8 * sizeof(unsigned long long int) - 1 - (size_t)__builtin_clzll(x))
#else
static inline size_t
libzahl_msb_nz_lu(unsigned long int x)
{
	size_t r = 0;
	for (; x; x >>= 1, r++);
	return r - 1;
}

static inline size_t
libzahl_msb_nz_llu(unsigned long long int x)
{
	size_t r = 0;
	for (; x; x >>= 1, r++);
	return r - 1;
}
#endif

#if defined(__GNUC__) || defined(__clang__)
# if INT64_MAX == LONG_MAX
#  define libzahl_add_overflow(rp, a, b)  __builtin_uaddl_overflow(a, b, rp)
# else
#  define libzahl_add_overflow(rp, a, b)  __builtin_uaddll_overflow(a, b, rp)
# endif
#else
static inline int
libzahl_add_overflow(zahl_char_t *rp, zahl_char_t a, zahl_char_t b)
{
	int carry = ZAHL_CHAR_MAX - a < b;
	*rp = a + b;
	return carry;
}
#endif

static inline void
zsplit_pz(z_t high, z_t low, z_t a, size_t delim)
{
	if (unlikely(zzero(a))) {
		SET_SIGNUM(high, 0);
		SET_SIGNUM(low, 0);
	} else {
		zsplit(high, low, a, delim);
	}
}

static inline void
zrsh_taint(z_t a, size_t bits)
{
	size_t i, chars, cbits;

	if (unlikely(!bits))
		return;
	if (unlikely(zzero(a)))
		return;

	chars = FLOOR_BITS_TO_CHARS(bits);

	if (unlikely(chars >= a->used || zbits(a) <= bits)) {
		SET_SIGNUM(a, 0);
		return;
	}

	bits = BITS_IN_LAST_CHAR(bits);
	cbits = BITS_PER_CHAR - bits;

	if (likely(chars)) {
		a->used -= chars;
		a->chars += chars;
	}

	if (unlikely(bits)) { /* This if statement is very important in C. */
		a->chars[0] >>= bits;
		for (i = 1; i < a->used; i++) {
			a->chars[i - 1] |= a->chars[i] << cbits;
			a->chars[i] >>= bits;
		}
		TRIM_NONZERO(a);
	}
}

static inline void
zswap_tainted_unsigned(z_t a, z_t b)
{
	z_t t;
	SWAP(a, b, t, used);
	SWAP(b, a, t, chars);
}

static inline void
zsplit_unsigned_fast_large_taint(z_t high, z_t low, z_t a, size_t n)
{
	n >>= LB_BITS_PER_CHAR;
	high->sign = 1;
	high->used = a->used - n;
	high->chars = a->chars + n;
#if 0
	TRIM_AND_ZERO(high);
#endif
	low->sign = 1;
	low->used = n;
	low->chars = a->chars;
	TRIM_AND_ZERO(low);
}

static inline void
zsplit_unsigned_fast_small_auto(z_t high, z_t low, z_t a, size_t n)
{
	zahl_char_t mask = 1;
	mask = (mask << n) - 1;

	high->sign = 1;
	high->used = 1;
	high->chars[0] = a->chars[0] >> n;
	if (a->used == 2) {
		high->chars[1] = a->chars[1] >> n;
		high->used += !!high->chars[1];
		n = BITS_PER_CHAR - n;
		high->chars[0] |= (a->chars[1] & mask) << n;
	}
#if 0
	if (unlikely(!high->chars[high->used - 1]))
		high->sign = 0;
#endif

	low->sign = 1;
	low->used = 1;
	low->chars[0] = a->chars[0] & mask;
	if (unlikely(!low->chars[0]))
		low->sign = 0;
}

/* Calls to these functions must be called in stack-order
 * For example, 
 * 
 *   zinit_temp(a);
 *   zinit_temp(b);
 *   zfree_temp(b);
 *   zinit_temp(c);
 *   zfree_temp(c);
 *   zfree_temp(a);
 * 
 * And not (swap the two last lines)
 * 
 *   zinit_temp(a);
 *   zinit_temp(b);
 *   zfree_temp(b);
 *   zinit_temp(c);
 *   zfree_temp(a);
 *   zfree_temp(c);
 * 
 * { */

static inline void
zinit_temp(z_t a)
{
	zinit(a);
	if (unlikely(libzahl_temp_stack_head == libzahl_temp_stack_end)) {
		size_t n = (size_t)(libzahl_temp_stack_end - libzahl_temp_stack);
		void* old = libzahl_temp_stack;
		libzahl_temp_stack = realloc(old, 2 * n * sizeof(*libzahl_temp_stack));
		if (check(!libzahl_temp_stack)) {
			libzahl_temp_stack = old;
			libzahl_memfailure();
		}
		libzahl_temp_stack_head = libzahl_temp_stack + n;
		libzahl_temp_stack_end = libzahl_temp_stack_head + n;
	}
	*libzahl_temp_stack_head++ = a;
}

static inline void
zfree_temp(z_t a)
{
	zfree(a);
	libzahl_temp_stack_head--;
}

/* } */

#define ZMEM_2OP_PRECISE(a, b, c, n, OP)                                      \
	do {                                                                  \
		zahl_char_t *a__ = (a);                                       \
		const zahl_char_t *b__ = (b);                                 \
		const zahl_char_t *c__ = (c);                                 \
		size_t i__, n__ = (n);                                        \
		if (n__ <= 4) {                                               \
			if (n__ >= 1)                                         \
				a__[0] = b__[0] OP c__[0];                    \
			if (n__ >= 2)                                         \
				a__[1] = b__[1] OP c__[1];                    \
			if (n__ >= 3)                                         \
				a__[2] = b__[2] OP c__[2];                    \
			if (n__ >= 4)                                         \
				a__[3] = b__[3] OP c__[3];                    \
		} else {                                                      \
			for (i__ = 0; (i__ += 4) < n__;) {                    \
				a__[i__ - 1] = b__[i__ - 1] OP c__[i__ - 1];  \
				a__[i__ - 2] = b__[i__ - 2] OP c__[i__ - 2];  \
				a__[i__ - 3] = b__[i__ - 3] OP c__[i__ - 3];  \
				a__[i__ - 4] = b__[i__ - 4] OP c__[i__ - 4];  \
			}                                                     \
			if (i__ > n__)                                        \
				for (i__ -= 4; i__ < n__; i__++)              \
					a__[i__] = b__[i__] OP c__[i__];      \
		}                                                             \
	} while (0)

#define ZMEM_2OP(a, b, c, n, OP)                                      \
	do {                                                          \
		zahl_char_t *a__ = (a);                               \
		const zahl_char_t *b__ = (b);                         \
		const zahl_char_t *c__ = (c);                         \
		size_t i__, n__ = (n);                                \
		for (i__ = 0; i__ < n__; i__ += 4) {                  \
			a__[i__ + 0] = b__[i__ + 0] OP c__[i__ + 0];  \
			a__[i__ + 1] = b__[i__ + 1] OP c__[i__ + 1];  \
			a__[i__ + 2] = b__[i__ + 2] OP c__[i__ + 2];  \
			a__[i__ + 3] = b__[i__ + 3] OP c__[i__ + 3];  \
		}                                                     \
	} while (0)

#define ZMEM_1OP(a, b, n, OP)                             \
	do {                                              \
		zahl_char_t *a__ = (a);                   \
		const zahl_char_t *b__ = (b);             \
		size_t i__, n__ = (n);                    \
		for (i__ = 0; i__ < n__; i__ += 4) {      \
			a__[i__ + 0] = OP(b__[i__ + 0]);  \
			a__[i__ + 1] = OP(b__[i__ + 1]);  \
			a__[i__ + 2] = OP(b__[i__ + 2]);  \
			a__[i__ + 3] = OP(b__[i__ + 3]);  \
		}                                         \
	} while (0)
