#include <hebimath.h>

#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BIGINT_LIBRARY "hebimath"
#define HEBIMATH

typedef hebi_int z_t[1];

static z_t _0, _1, _2, _add_sub_a, _add_sub_b, _cmp_b, _pow_a, _pow_m;
static z_t _trunc_btest_a, _bits_lsb_a, _str_a, _str_b, _str_m, _bset_a;
static z_t _logic_a, _logic_b, _logic_x, _not_a, _not_b, _gcd_a, _gcd_b;
static z_t _shift_b, _div_a, _div_b, _divmod;
static int error;
static jmp_buf jbuf;

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

#ifdef ZAHL_UNSAFE
# define try(expr) (expr)
#else
# define try(expr) do if ((error = (expr))) longjmp(jbuf, 1); while (0)
#endif

#define zsave(a, s) zstr(a, s, sizeof(s) - 1)
#define zload(a, s) zsets(a, s)

static inline void
zsetup(jmp_buf env)
{
	*jbuf = *env;
	hebi_init(_0);
	hebi_init(_1);
	hebi_init(_2);
	hebi_init(_add_sub_a);
	hebi_init(_add_sub_b);
	hebi_init(_cmp_b);
	hebi_init(_pow_a);
	hebi_init(_pow_m);
	hebi_init(_trunc_btest_a);
	hebi_init(_bits_lsb_a);
	hebi_init(_str_a);
	hebi_init(_str_b);
	hebi_init(_str_m);
	hebi_init(_bset_a);
	hebi_init(_logic_a);
	hebi_init(_logic_b);
	hebi_init(_logic_x);
	hebi_init(_not_a);
	hebi_init(_not_b);
	hebi_init(_gcd_a);
	hebi_init(_gcd_b);
	hebi_init(_shift_b);
	hebi_init(_div_a);
	hebi_init(_div_b);
	hebi_init(_divmod);
	try(hebi_set_u(_0, 0));
	try(hebi_set_u(_1, 1));
	try(hebi_set_u(_2, 2));
}

static inline void
zunsetup(void)
{
	hebi_destroy(_0);
	hebi_destroy(_1);
	hebi_destroy(_2);
	hebi_destroy(_add_sub_a);
	hebi_destroy(_add_sub_b);
	hebi_destroy(_cmp_b);
	hebi_destroy(_pow_a);
	hebi_destroy(_pow_m);
	hebi_destroy(_trunc_btest_a);
	hebi_destroy(_bits_lsb_a);
	hebi_destroy(_str_a);
	hebi_destroy(_str_b);
	hebi_destroy(_str_m);
	hebi_destroy(_bset_a);
	hebi_destroy(_logic_a);
	hebi_destroy(_logic_b);
	hebi_destroy(_logic_x);
	hebi_destroy(_not_a);
	hebi_destroy(_not_b);
	hebi_destroy(_gcd_a);
	hebi_destroy(_gcd_b);
	hebi_destroy(_shift_b);
	hebi_destroy(_div_a);
	hebi_destroy(_div_b);
	hebi_destroy(_divmod);
}

static inline void
zperror(const char *str)
{
	const char *serr;
	switch (error) {
	case hebi_badrange: serr = "hebi_badrange"; break;
	case hebi_badvalue: serr = "hebi_badvalue"; break;
	case hebi_nomem:    serr = "hebi_nomem";    break;
	default:            serr = "unknown error"; break;
	}
	if (str && *str)
		fprintf(stderr, "%s: %s\n", str, serr);
	else
		fprintf(stderr, "%s\n", serr);
}

static inline void
zinit(z_t a)
{
	hebi_init(a);
}

static inline void
zfree(z_t a)
{
	hebi_destroy(a);
}

static inline void
zset(z_t r, const z_t a)
{
	try(hebi_set(r, a));
}

static inline void
zsetu(z_t r, unsigned long long int a)
{
	try(hebi_set_u(r, a));
}

static inline void
zseti(z_t r, long long int a)
{
	try(hebi_set_i(r, a));
}

static inline void
zneg(z_t r, const z_t a)
{
	try(hebi_neg(r, a));
}

static inline void
zabs(z_t r, const z_t a)
{
	if (hebi_sign(a) < 0)
		zneg(r, a);
	else
		zset(r, a);
}

static inline void
zadd(z_t r, const z_t a, const z_t b)
{
	try(hebi_add(r, a, b));
}

static inline void
zsub(z_t r, const z_t a, const z_t b)
{
	try(hebi_sub(r, a, b));
}

static inline void
zadd_unsigned(z_t r, const z_t a, const z_t b)
{
	zabs(_add_sub_a, a);
	zabs(_add_sub_b, b);
	zadd(r, _add_sub_a, _add_sub_b);
}

static inline void
zsub_unsigned(z_t r, const z_t a, const z_t b)
{
	zabs(_add_sub_a, a);
	zabs(_add_sub_b, b);
	zsub(r, _add_sub_a, _add_sub_b);
}

static inline int
zeven(const z_t a)
{
	return ~hebi_get_u(a) & 1;
}

static inline int
zodd(const z_t a)
{
	return hebi_get_u(a) & 1;
}

static inline int
zeven_nonzero(const z_t a)
{
	return zeven(a);
}

static inline int
zodd_nonzero(const z_t a)
{
	return zodd(a);
}

static inline void
zswap(z_t a, z_t b)
{
	hebi_swap(a, b);
}

static inline int
zcmpmag(const z_t a, const z_t b)
{
	return hebi_cmp_mag(a, b);
}

static inline int
zcmp(const z_t a, const z_t b)
{
	return hebi_cmp(a, b);
}

static inline int
zcmpi(const z_t a, long long int b)
{
	zseti(_cmp_b, b);
	return zcmp(a, _cmp_b);
}

static inline int
zcmpu(const z_t a, long long int b)
{
	zsetu(_cmp_b, b);
	return zcmp(a, _cmp_b);
}

static inline int
zsignum(const z_t a)
{
	return zcmp(a, _0);
}

static inline int
zzero(const z_t a)
{
	return !zsignum(a);
}

static inline void
zmul(z_t r, const z_t a, const z_t b)
{
	try(hebi_mul(r, a, b));
}

static inline void
zsqr(z_t r, const z_t a)
{
	zmul(r, a, a);
}

static inline void
zsets(z_t a, const char *s)
{
	try(hebi_set_str(a, s, 0, 10));
}

static inline void
zdivmod(z_t q, z_t r, const z_t a, const z_t b)
{
	try(hebi_div(q, r, a, b));
}

static inline void
zdiv(z_t q, const z_t a, const z_t b)
{
	zdivmod(q, 0, a, b);
}

static inline void
zmod(z_t r, const z_t a, const z_t b)
{
	zdivmod(_divmod, r, a, b);
}

static inline void
zmodmul(z_t r, const z_t a, const z_t b, const z_t m)
{
	zmul(r, a, b);
	zmod(r, r, m);
}

static inline void
zmodsqr(z_t r, const z_t a, const z_t m)
{
	zsqr(r, a);
	zmod(r, r, m);
}

static inline void
zpowu(z_t r, const z_t a, unsigned long long int b)
{
	if (!b) {
		if (zzero(a)) {
			error = hebi_badvalue;
			longjmp(jbuf, 1);
		}
		zsetu(r, 1);
		return;
	}

	zset(_pow_a, a);
	zsetu(r, 1);
	for (; b; b >>= 1) {
		if (b & 1)
			zmul(r, r, _pow_a);
		zsqr(_pow_a, _pow_a);
	}
}

static inline void
zpow(z_t r, const z_t a, const z_t b)
{
	zpowu(r, a, hebi_get_u(b));
}

static inline void
zmodpowu(z_t r, const z_t a, unsigned long long int b, const z_t m)
{
	if (!b) {
		if (zzero(a) || zzero(m)) {
			error = hebi_badvalue;
			longjmp(jbuf, 1);
		}
		zsetu(r, 1);
		return;
	} else if (zzero(m)) {
		error = hebi_badvalue;
		longjmp(jbuf, 1);
	}

	zmod(_pow_a, a, m);
	zset(_pow_m, m);
	zsetu(r, 1);
	for (; b; b >>= 1) {
		if (b & 1)
			zmodmul(r, r, _pow_a, _pow_m);
		zmodsqr(_pow_a, _pow_a, _pow_m);
	}
}

static inline void
zmodpow(z_t r, const z_t a, const z_t b, const z_t m)
{
	zmodpowu(r, a, hebi_get_u(b), m);
}

static inline void
zlsh(z_t r, const z_t a, size_t b)
{
	try(hebi_shl(r, a, b));
}

static inline void
zrsh(z_t r, const z_t a, size_t b)
{
	try(hebi_shr(r, a, b));
}

static inline void
ztrunc(z_t r, const z_t a, size_t b)
{
	zrsh(_trunc_btest_a, a, b);
	zlsh(_trunc_btest_a, _trunc_btest_a, b);
	zsub(r, a, _trunc_btest_a);
}

static inline int
zbtest(z_t a, size_t b)
{
	zlsh(_trunc_btest_a, a, b);
	return zodd(a);
}

static inline size_t
zbits(const z_t a)
{
	hebi_uword x = x;
	size_t rc = 0;
	zset(_bits_lsb_a, a);
	while (zsignum(_bits_lsb_a)) {
		x = hebi_get_u(_bits_lsb_a);
		zrsh(_bits_lsb_a, _bits_lsb_a, 8 * sizeof(x));
		rc += 8 * sizeof(x);
	}
	if (rc)
		rc -= 8 * sizeof(x);
	while (x) {
		x >>= 1;
		rc += 1;
	}
	return rc;
}

static inline size_t
zlsb(const z_t a)
{
	hebi_uword x;
	size_t rc = 0;
	if (zzero(a))
		return SIZE_MAX;
	zset(_bits_lsb_a, a);
	while (!(x = hebi_get_u(_bits_lsb_a))) {
		zrsh(_bits_lsb_a, _bits_lsb_a, 8 * sizeof(x));
		rc += 8 * sizeof(x);
	}
	while (~x & 1) {
		x >>= 1;
		rc += 1;
	}
	return rc;
}

static inline void
zptest(z_t w, const z_t a, int t)
{
	static int gave_up = 0;
	if (!gave_up) {
		gave_up = 1;
		printf("I'm sorry, primality test has not been implemented.\n");
	}
	(void) w;
	(void) a;
	(void) t;
}

static inline size_t
zstr_length(const z_t a, unsigned long long int radix)
{
	size_t size_total = 1, size_temp;
	zset(_str_a, a);
	while (!zzero(_str_a)) {
		zsetu(_str_m, radix);
		zset(_str_b, _str_m);
		size_temp = 1;
		while (zcmpmag(_str_m, _str_a) <= 0) {
			zset(_str_b, _str_m);
			zsqr(_str_m, _str_m);
			size_temp <<= 1;
		}
		size_temp >>= 1;
		size_total += size_temp;
		zdiv(_str_a, _str_a, _str_b);
	}
	return size_total + (zsignum(a) < 0);
}

static inline char *
zstr(const z_t a, char *s, size_t n)
{
	if (!n)
		n = zstr_length(a, 10) * 2 + 1;
	try(hebi_get_str(s, n, a, 10));
	return s;
}

static inline void
zsplit(z_t high, z_t low, const z_t a, size_t brk)
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
zbset(z_t r, const z_t a, size_t bit, int mode)
{
	zrsh(_bset_a, a, bit);
	if (mode && zeven(_bset_a)) {
		zlsh(_bset_a, _1, bit);
		zadd(r, a, _bset_a);
	} else if (mode <= 0 && zodd(_bset_a)) {
		zlsh(_bset_a, _1, bit);
		zsub(r, a, _bset_a);
	} else {
		zset(r, a);
	}
}

static inline void
zrand(z_t r, int dev, int dist, const z_t n)
{
	static int gave_up[] = {0, 0, 0};
	if (!gave_up[dist]) {
		gave_up[dist] = 1;
		printf("I'm sorry, prng has not been implemented.\n");
	}
	(void) r;
	(void) dev;
	(void) n;
}

static inline void
zand(z_t r, const z_t a, const z_t b)
{
	int neg = hebi_sign(a) < 0 && hebi_sign(b) < 0;
	hebi_uword x;
	size_t i = 0;
	zset(_logic_a, a);
	zset(_logic_b, b);
	zsetu(r, 0);
	while (zsignum(_logic_a) && zsignum(_logic_b)) {
		x = hebi_get_u(_logic_a) & hebi_get_u(_logic_b);
		zsetu(_logic_x, x);
		zlsh(_logic_x, _logic_x, i++ * 8 * sizeof(x));
		zadd(r, r, _logic_x);
		zrsh(_logic_a, _logic_a, 8 * sizeof(x));
		zrsh(_logic_b, _logic_b, 8 * sizeof(x));
	}
	if (neg)
		zneg(r, r);
}

static inline void
zor(z_t r, const z_t a, const z_t b)
{
	int neg = hebi_sign(a) < 0 || hebi_sign(b) < 0;
	hebi_uword x;
	size_t i = 0;
	zset(_logic_a, a);
	zset(_logic_b, b);
	zsetu(r, 0);
	while (zsignum(_logic_a) || zsignum(_logic_b)) {
		x = hebi_get_u(_logic_a) | hebi_get_u(_logic_b);
		zsetu(_logic_x, x);
		zlsh(_logic_x, _logic_x, i++ * 8 * sizeof(x));
		zadd(r, r, _logic_x);
		zrsh(_logic_a, _logic_a, 8 * sizeof(x));
		zrsh(_logic_b, _logic_b, 8 * sizeof(x));
	}
	if (neg)
		zneg(r, r);
}

static inline void
zxor(z_t r, const z_t a, const z_t b)
{
	int neg = (hebi_sign(a) < 0) ^ (hebi_sign(b) < 0);
	hebi_uword x;
	size_t i = 0;
	zset(_logic_a, a);
	zset(_logic_b, b);
	zsetu(r, 0);
	while (zsignum(_logic_a) || zsignum(_logic_b)) {
		x = hebi_get_u(_logic_a) ^ hebi_get_u(_logic_b);
		zsetu(_logic_x, x);
		zlsh(_logic_x, _logic_x, i++ * 8 * sizeof(x));
		zadd(r, r, _logic_x);
		zrsh(_logic_a, _logic_a, 8 * sizeof(x));
		zrsh(_logic_b, _logic_b, 8 * sizeof(x));
	}
	if (neg)
		zneg(r, r);
}

static inline void
zgcd(z_t r, const z_t a, const z_t b)
{
	size_t shifts, a_lsb, b_lsb;
	int neg, cmpmag;

	if (zzero(a)) {
		if (r != b)
			zset(r, b);
		return;
	}
	if (zzero(b)) {
		if (r != a)
			zset(r, a);
		return;
	}

	neg = hebi_sign(a) < 0 && hebi_sign(b) < 0;

	a_lsb = zlsb(a);
	b_lsb = zlsb(b);
	shifts = a_lsb < b_lsb ? a_lsb : b_lsb;
	zrsh(_gcd_a, a, a_lsb);
	zrsh(_gcd_b, b, b_lsb);

	for (;;) {
		if ((cmpmag = zcmpmag(_gcd_a, _gcd_b)) >= 0) {
			if (cmpmag == 0)
				break;
			zswap(_gcd_a, _gcd_b);
		}
		zsub(_gcd_b, _gcd_b, _gcd_a);
		zrsh(_gcd_b, _gcd_b, zlsb(_gcd_b));
	}

	zlsh(r, _gcd_a, shifts);
	if (neg)
		zneg(r, r);
}

static inline void
znot(z_t r, const z_t a)
{
	size_t bits = zbits(a);
	zsetu(_not_b, 0);
	zsetu(_not_a, 1);
	zlsh(_not_a, _not_a, bits);
	zadd(_not_b, _not_b, _logic_a);
	zsub(_not_b, _not_b, _1);
	zxor(r, a, _not_b);
	zneg(r, r);
}

/* Prototype declared, but implementation missing, in hebimath */

int
hebi_shl(hebi_int *r, const hebi_int *a, unsigned int b)
{
	zpowu(_shift_b, _2, b);
	zmul(r, a, _shift_b);
	return hebi_success;
}

int
hebi_shr(hebi_int *r, const hebi_int *a, unsigned int b)
{
	zpowu(_shift_b, _2, b);
	zdiv(r, a, _shift_b);
	return hebi_success;
}

int
hebi_div(hebi_int *q, hebi_int *r, const hebi_int *a, const hebi_int *b)
{
	int neg = zsignum(a) < 0;
	zset(q, _0);
	zabs(_div_a, a);
	zabs(_div_b, b);
	if (zzero(b)) {
		error = hebi_badvalue;
		longjmp(jbuf, 1);
	}
	while (zcmpmag(_div_a, _div_b) >= 0) {
		zadd(q, q, _1);
		zsub(_div_a, _div_a, _div_b);
	}
	if (neg)
		zneg(q, q);
	if (r)
		zset(r, _div_a);
	return hebi_success;
}
