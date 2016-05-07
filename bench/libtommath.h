#include <tommath.h>

#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BIGINT_LIBRARY "libtommath"

#define FAST_RANDOM        0
#define SECURE_RANDOM      0
#define DEFAULT_RANDOM     0
#define FASTEST_RANDOM     0
#define LIBC_RAND_RANDOM   0
#define LIBC_RANDOM_RANDOM 0
#define LIBC_RAND48_RANDOM 0
#define QUASIUNIFORM       0
#define UNIFORM            1
#define MODUNIFORM         2

typedef mp_int z_t[1];

static z_t _0, _1, _a, _b;
static int _tmp, error;
static jmp_buf jbuf;

#ifdef ZAHL_UNSAFE
# define try(expr) (expr)
#else
# define try(expr) do if ((error = (expr))) longjmp(jbuf, 1); while (0)
#endif

static inline void
zsetup(jmp_buf env)
{
	*jbuf = *env;
	try(mp_init_set_int(_0, 0));
	try(mp_init_set_int(_1, 1));
	try(mp_init(_a));
	try(mp_init(_b));
}

static inline void
zunsetup(void)
{
	mp_clear(_0);
	mp_clear(_1);
	mp_clear(_a);
	mp_clear(_b);
}

static inline void
zperror(const char *str)
{
	if (str && *str)
		fprintf(stderr, "%s: %s\n", str, mp_error_to_string(error));
	else
		fprintf(stderr, "%s\n", mp_error_to_string(error));
}

static inline void
zinit(z_t a)
{
	try(mp_init(a));
}

static inline void
zfree(z_t a)
{
	mp_clear(a);
}

static inline void
zset(z_t r, z_t a)
{
	try(mp_copy(a, r));
}

static inline void
zneg(z_t r, z_t a)
{
	try(mp_neg(a, r));
}

static inline void
zabs(z_t r, z_t a)
{
	try(mp_abs(a, r));
}

static inline void
zadd(z_t r, z_t a, z_t b)
{
	try(mp_add(a, b, r));
}

static inline void
zsub(z_t r, z_t a, z_t b)
{
	try(mp_sub(a, b, r));
}

static inline void
zadd_unsigned(z_t r, z_t a, z_t b)
{
	zabs(_a, a);
	zabs(_b, b);
	zadd(r, _a, _b);
}

static inline void
zsub_unsigned(z_t r, z_t a, z_t b)
{
	zabs(_a, a);
	zabs(_b, b);
	zsub(r, _a, _b);
}

static inline size_t
zbits(z_t a)
{
	return mp_count_bits(a);
}

static inline size_t
zlsb(z_t a)
{
	return mp_cnt_lsb(a);
}

static inline int
zeven(z_t a)
{
	return mp_iseven(a);
}

static inline int
zodd(z_t a)
{
	return mp_isodd(a);
}

static inline int
zeven_nonzero(z_t a)
{
	return zeven(a);
}

static inline int
zodd_nonzero(z_t a)
{
	return zodd(a);
}

static inline int
zzero(z_t a)
{
	return mp_iszero(a);
}

static inline void
zand(z_t r, z_t a, z_t b)
{
	try(mp_and(a, b, r));
}

static inline void
zor(z_t r, z_t a, z_t b)
{
	try(mp_or(a, b, r));
}

static inline void
zxor(z_t r, z_t a, z_t b)
{
	try(mp_xor(a, b, r));
}

static inline void
znot(z_t r, z_t a)
{
	try(mp_2expt(_a, (int)zbits(a)));
	try(mp_sub_d(_a, 1, _a));
	zand(r, a, _a);
	zneg(r, r);
}

static inline int
zbtest(z_t a, size_t bit)
{
	try(mp_2expt(_b, (int)bit));
	zand(_b, a, _b);
	return !zzero(_b);
}

static inline void
zbset(z_t r, z_t a, size_t bit, int mode)
{
	if (mode > 0) {
		try(mp_2expt(_b, (int)bit));
		zor(r, a, _b);
	} else if (mode < 0 || zbtest(a, bit)) {
		try(mp_2expt(_b, (int)bit));
		zxor(r, a, _b);
	}
}

static inline void
zswap(z_t a, z_t b)
{
	mp_exch(a, b);
}

static inline void
zlsh(z_t r, z_t a, size_t b)
{
	try(mp_mul_2d(a, (int)b, r));
}

static inline void
zrsh(z_t r, z_t a, size_t b)
{
	try(mp_div_2d(a, (int)b, r, 0));
}

static inline void
ztrunc(z_t r, z_t a, size_t b)
{
	try(mp_mod_2d(a, (int)b, r));
}

static inline void
zsplit(z_t high, z_t low, z_t a, size_t brk)
{
	if (low == a) {
		zrsh(high, a, brk);
		ztrunc(low, a, brk);
	} else {
		ztrunc(low, a, brk);
		zrsh(high, a, brk);
	}
}

static inline void
zsetu(z_t r, unsigned long long int val)
{
	try(mp_set_long_long(r, val));
}

static inline void
zseti(z_t r, long long int val)
{
	if (val >= 0) {
		zsetu(r, (unsigned long long int)val);
	} else {
		zsetu(r, (unsigned long long int)-val);
		zneg(r, r);
	}
}

static inline int
zcmpmag(z_t a, z_t b)
{
	return mp_cmp_mag(a, b);
}

static inline int
zcmp(z_t a, z_t b)
{
	return mp_cmp(a, b);
}

static inline int
zcmpi(z_t a, long long int b)
{
	zseti(_b, b);
	return zcmp(a, _b);
}

static inline int
zcmpu(z_t a, unsigned long long int b)
{
	zsetu(_b, b);
	return zcmp(a, _b);
}

static inline int
zsignum(z_t a)
{
	return zcmp(a, _0);
}

static inline void
zgcd(z_t r, z_t a, z_t b)
{
	try(mp_gcd(a, b, r));
}

static inline void
zmul(z_t r, z_t a, z_t b)
{
	try(mp_mul(a, b, r));
}

static inline void
zsqr(z_t r, z_t a)
{
	try(mp_sqr(a, r));
}

static inline void
zmodmul(z_t r, z_t a, z_t b, z_t m)
{
	try(mp_mulmod(a, b, m, r));
}

static inline void
zmodsqr(z_t r, z_t a, z_t m)
{
	try(mp_sqrmod(a, m, r));
}

static inline void
zpow(z_t r, z_t a, z_t b)
{
	try(mp_expt_d(a, (mp_digit)mp_get_int(b), r));
}

static inline void
zpowu(z_t r, z_t a, unsigned long long int b)
{
	try(mp_expt_d(a, (mp_digit)b, r));
}

static inline void
zmodpow(z_t r, z_t a, z_t b, z_t m)
{
	try(mp_exptmod(a, b, m, r));
}

static inline void
zmodpowu(z_t r, z_t a, unsigned long long int b, z_t m)
{
	try(mp_set_int(_b, b));
	try(mp_exptmod(a, _b, m, r));
}

static inline void
zsets(z_t a, const char *s)
{
	try(mp_read_radix(a, s, 10));
}

static inline size_t
zstr_length(z_t a, size_t b)
{
	try(mp_radix_size(a, b, &_tmp));
	return _tmp;
}

static inline char *
zstr(z_t a, char *s, size_t n)
{
	try(mp_toradix(a, s, 10));
	return s;
	(void) n;
}

static inline int
zptest(z_t w, z_t a, int t)
{
	try(mp_prime_is_prime(a, t, &_tmp));
	return _tmp;
	(void) w; /* Note, the witness is not returned. */
}

static inline size_t
zsave(z_t a, char *b)
{
	_tmp = !b ? mp_signed_bin_size(a) : mp_to_signed_bin(a, (unsigned char *)b);
	return _tmp;
}

static inline size_t
zload(z_t a, const char *b) /* Note, requires that zsave was called directly prior. */
{
	return mp_read_signed_bin(a, (const unsigned char *)b, _tmp);
}

static inline void
zdiv(z_t r, z_t a, z_t b)
{
	try(mp_div(a, b, r, 0));
}

static inline void
zmod(z_t r, z_t a, z_t b)
{
	try(mp_mod(a, b, r));
}

static inline void
zdivmod(z_t q, z_t r, z_t a, z_t b)
{
	try(mp_div(a, b, q, r));
}

static inline void
zrand(z_t r, int dev, int dist, z_t n)
{
	static int gave_up = 0;
	int bits;
	(void) dev;

	if (zzero(n)) {
		mp_zero(r);
		return;
	}
	if (zsignum(n) < 0) {
		return;
	}

	bits = zbits(n);

	switch (dist) {
	case QUASIUNIFORM:
		try(mp_rand(r, bits));
		zadd(r, r, _1);
		zmul(r, r, n);
		zrsh(r, r, bits);
		break;

	case UNIFORM:
		if (!gave_up) {
			gave_up = 1;
			printf("I'm sorry, this is too difficult, I give up.\n");
		}
		break;

	case MODUNIFORM:
		try(mp_rand(r, bits));
		if (zcmp(r, n) > 0)
			zsub(r, r, n);
		break;

	default:
		abort();
	}
}
