/* See LICENSE file for copyright and license details. */
#include "../zahl.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* clang pretends to be GCC... */
#if defined(__GNUC__) && defined(__clang__)
# undef __GNUC__
#endif

#define BITS_PER_CHAR                64
#define LB_BITS_PER_CHAR             6
#define ZAHL_CHAR_MAX                UINT64_MAX

#define FLOOR_BITS_TO_CHARS(bits)    ((bits) >> LB_BITS_PER_CHAR)
#define CEILING_BITS_TO_CHARS(bits)  (((bits) + (BITS_PER_CHAR - 1)) >> LB_BITS_PER_CHAR)
#define BITS_IN_LAST_CHAR(bits)      ((bits) & (BITS_PER_CHAR - 1))

#if defined(__GNUC__)
# define O0     __attribute__((optimize("O0")))
# define O1     __attribute__((optimize("O1")))
# define O2     __attribute__((optimize("O2")))
# define O3     __attribute__((optimize("O3")))
# define Ofast  __attribute__((optimize("Ofast")))
# define Os     __attribute__((optimize("Os")))
# define Oz     __attribute__((optimize("Os")))
#elif defined(__clang__)
# define O0     __attribute__((optnone))
# define O1     /* Don't know how. */
# define O2     /* Don't know how. */
# define O3     /* Don't know how. */
# define Ofast  /* Don't know how. */
# define Os     /* Don't know how. */
# define Oz     /* Don't know how. */
#else
# define O0     /* Don't know how. */
# define O1     /* Don't know how. */
# define O2     /* Don't know how. */
# define O3     /* Don't know how. */
# define Ofast  /* Don't know how. */
# define Os     /* Don't know how. */
# define Oz     /* Don't know how. */
#endif

#define LIST_TEMPS\
	X(libzahl_tmp_cmp)\
	X(libzahl_tmp_str_num)\
	X(libzahl_tmp_str_mag)\
	X(libzahl_tmp_str_div)\
	X(libzahl_tmp_str_rem)\
	X(libzahl_tmp_gcd_u)\
	X(libzahl_tmp_gcd_v)\
	X(libzahl_tmp_sub)\
	X(libzahl_tmp_modmul)\
	X(libzahl_tmp_div)\
	X(libzahl_tmp_mod)\
	X(libzahl_tmp_pow_b)\
	X(libzahl_tmp_pow_c)\
	X(libzahl_tmp_pow_d)\
	X(libzahl_tmp_modsqr)\
	X(libzahl_tmp_divmod_a)\
	X(libzahl_tmp_divmod_b)\
	X(libzahl_tmp_divmod_d)\
	X(libzahl_tmp_ptest_x)\
	X(libzahl_tmp_ptest_a)\
	X(libzahl_tmp_ptest_d)\
	X(libzahl_tmp_ptest_n1)\
	X(libzahl_tmp_ptest_n4)

#define LIST_CONSTS\
	X(libzahl_const_1e19, zsetu, 10000000000000000000ULL) /* The largest power of 10 < 2⁶⁴. */\
	X(libzahl_const_1,    zsetu, 1)\
	X(libzahl_const_2,    zsetu, 2)\
	X(libzahl_const_4,    zsetu, 4)

#define X(x)  extern z_t x;
LIST_TEMPS
#undef X
#define X(x, f, v)  extern z_t x;
LIST_CONSTS
#undef X

extern z_t libzahl_tmp_divmod_ds[BITS_PER_CHAR];
extern jmp_buf libzahl_jmp_buf;
extern int libzahl_set_up;
extern int libzahl_error;
extern zahl_char_t **libzahl_pool[sizeof(size_t) * 8];
extern size_t libzahl_pool_n[sizeof(size_t) * 8];
extern size_t libzahl_pool_alloc[sizeof(size_t) * 8];

#if defined(__GNUC__) || defined(__clang__)
# define likely(value)               __builtin_expect(!!(value), 1)
# define unlikely(value)             __builtin_expect(!!(value), 0)
#else
# define likely(value)               (value)
# define unlikely(value)             (value)
#endif

#define libzahl_failure(error)       (libzahl_error = (error), longjmp(libzahl_jmp_buf, 1))
#define SET_SIGNUM(a, signum)        ((a)->sign = (signum))
#define SET(a, b)                    do { if ((a) != (b)) zset(a, b); } while (0)
#define ENSURE_SIZE(a, n)            do { if ((a)->alloced < (n)) libzahl_realloc(a, (n)); } while (0)
#define MIN(a, b)                    ((a) < (b) ? (a) : (b))
#define MAX(a, b)                    ((a) > (b) ? (a) : (b))
#define TRIM(a)                      for (; (a)->used && !(a)->chars[(a)->used - 1]; (a)->used--)
#define TRIM_NONZERO(a)              for (; !(a)->chars[(a)->used - 1]; (a)->used--)
#define TRIM_AND_ZERO(a)             do { TRIM(a); if (!(a)->used) SET_SIGNUM(a, 0); } while (0)
#define TRIM_AND_SIGN(a, s)          do { TRIM(a); SET_SIGNUM(a, (a)->used ? (s) : 0); } while (0)
#define znegative(a)                 (zsignum(a) < 0)
#define znegative1(a, b)             ((zsignum(a) | zsignum(b)) < 0)
#define znegative2(a, b)             ((zsignum(a) & zsignum(b)) < 0)
#define zpositive(a)                 (zsignum(a) > 0)
#define zpositive1(a, b)             (zpositive(a) + zpositive(b) > 0)
#define zpositive2(a, b)             (zsignum(a) + zsignum(b) == 2)
#define zzero1(a, b)                 (zzero(a) || zzero(b))
#define zzero2(a, b)                 (!(zsignum(a) | zsignum(b)))
#define zmemmove(d, s, n)            memmove((d), (s), (n) * sizeof(zahl_char_t))

void libzahl_realloc(z_t a, size_t need);

static inline void
zmemcpy(zahl_char_t *restrict d, const zahl_char_t *restrict s, register size_t n)
{
	while (n--)
		d[n] = s[n];
}

static inline void
zmemset(zahl_char_t *a, register zahl_char_t v, register size_t n)
{
	while (n--)
		a[n] = v;
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
# define libzahl_msb_nz_lu(x)        (8 * sizeof(unsigned long int) - (size_t)__builtin_clzl(x));
# define libzahl_msb_nz_llu(x)       (8 * sizeof(unsigned long long int) - (size_t)__builtin_clzll(x));
#else
static inline size_t
libzahl_msb_nz_lu(unsigned long int x)
{
	size_t r = 0;
	for (; x; x >>= 1, r++);
	return r;
}

static inline size_t
libzahl_msb_nz_llu(unsigned long long int x)
{
	size_t r = 0;
	for (; x; x >>= 1, r++);
	return r;
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
