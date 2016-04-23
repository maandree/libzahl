#include <hebimath.h>

#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BIGINT_LIBRARY "hebimath"

typedef hebi_int z_t[1];

static z_t _0, _1, _a, _b, _m;

static void
zsetup(jmp_buf env)
{
	(void) env;
	hebi_set_u(_0, 0);
	hebi_set_u(_1, 1);
	hebi_init(_a);
	hebi_init(_b);
	hebi_init(_m);
}

static void
zunsetup(void)
{
	hebi_destroy(_0);
	hebi_destroy(_1);
	hebi_destroy(_a);
	hebi_destroy(_b);
	hebi_destroy(_m);
}

#define FAST_RANDOM             0
#define SECURE_RANDOM           0
#define DEFAULT_RANDOM          0
#define FASTEST_RANDOM          0
#define LIBC_RAND_RANDOM        0
#define LIBC_RANDOM_RANDOM      0
#define LIBC_RAND48_RANDOM      0
#define QUASIUNIFORM            0
#define UNIFORM                 1
#define MODUNIFORM              2

#define zperror(x)              ((void)0)
#define zinit                   hebi_init
#define zfree                   hebi_destroy

#define zset                    hebi_set
#define zsetu                   hebi_set_u
#define zseti                   hebi_set_i
#define zneg                    hebi_neg
#define zabs(r, a)              (hebi_sign(a) < 0 ? zneg(r, a) : zset(r, a))
#define zadd_unsigned(r, a, b)  (zabs(_a, a), zabs(_b, b), zadd(r, _a, _b))
#define zsub_unsigned(r, a, b)  (zabs(_a, a), zabs(_b, b), zsub(r, _a, _b))
#define zadd                    hebi_add
#define zsub                    hebi_sub
#define zbtest(a, b)            (hebi_shl(_a, a, b), (hebi_get_u(a) & 1))
#define zeven(a)                (~hebi_get_u(a) & 1)
#define zodd(a)                 (hebi_get_u(a) & 1)
#define zeven_nonzero           zeven
#define zodd_nonzero            zodd
#define zzero(a)                (!zsignum(a))
#define zsignum(a)              (zcmp(a, _0))
#define zswap                   hebi_swap
#define zlsh                    hebi_shl
#define zrsh                    hebi_shr
#define ztrunc(r, a, b)         (hebi_shr(_a, a, b), hebi_shl(_a, _a, b), hebi_sub(r, a, _a))
#define zcmpmag                 hebi_cmp_mag
#define zcmp                    hebi_cmp
#define zcmpi(a, b)             (zseti(_b, b), zcmp(a, _b))
#define zcmpu(a, b)             (zsetu(_b, b), zcmp(a, _b))
#define zmul                    hebi_mul
#define zsqr(r, a)              hebi_mul(r, a, a)
#define zmodmul(r, a, b, m)     (zmul(r, a, b), zmod(r, r, m))
#define zmodsqr(r, a, m)        (zsqr(r, a), zmod(r, r, m))
#define zpow(r, a, b)           zpowu(r, a, hebi_get_u(b))
#define zmodpow(r, a, b, m)     zmodpowu(r, a, hebi_get_u(b), m)
#define zsets(a, s)             hebi_set_str(a, s, 0, 10)
#define zstr(a, s)              hebi_get_str(s, sizeof(s), a, 10) /* Assumes s is [], not * */
#define zdiv(q, a, b)           hebi_div(q, 0, a, b)
#define zmod(r, a, b)           hebi_div(_a, r, a, b)
#define zdivmod(q, r, a, b)     hebi_div(q, r, a, b)
#define zsave(a, s)             zstr(a, s)
#define zload(a, s)             zsets(a, s)

static size_t
zbits(z_t a)
{
	hebi_uword x = x;
	size_t rc = 0;
	zset(_a, a);
	while (zsignum(_a)) {
		x = hebi_get_u(_a);
		zrsh(_a, _a, 8 * sizeof(x));
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

static size_t
zlsb(z_t a)
{
	hebi_uword x;
	size_t rc = 0;
	if (zzero(a))
		return SIZE_MAX;
	zset(_a, a);
	while (!(x = hebi_get_u(_a))) {
		zrsh(_a, _a, 8 * sizeof(x));
		rc += 8 * sizeof(x);
	}
	while (~x & 1) {
		x >>= 1;
		rc += 1;
	}
	return rc;
}

static void
zptest(z_t w, z_t a, int t)
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

static void
zpowu(z_t r, z_t a, unsigned long long int b)
{
	int neg = zsignum(a) < 0;
	zset(_a, a);
	zsetu(r, 1);
	for (; b; b >>= 1) {
		if (b & 1)
			zmul(r, r, _a);
		zsqr(_a, _a);
	}
	if (neg)
		zneg(r, r);
}

static void
zmodpowu(z_t r, z_t a, unsigned long long int b, z_t m)
{
	int neg = zsignum(a) < 0;
	zset(_a, a);
	zset(_m, m);
	zsetu(r, 1);
	for (; b; b >>= 1) {
		if (b & 1)
			zmodmul(r, r, _a, _m);
		zmodsqr(_a, _a, _m);
	}
	if (neg)
		zneg(r, r);
}

static size_t
zstr_length(z_t a, unsigned long long int radix)
{
	size_t size_total = 1, size_temp;
	zset(_a, a);
	while (!zzero(_a)) {
		zsetu(_m, radix);
		zset(_b, _m);
		size_temp = 1;
		while (zcmpmag(_m, _a) <= 0) {
			zset(_b, _m);
			zsqr(_m, _m);
			size_temp <<= 1;
		}
		size_temp >>= 1;
		size_total += size_temp;
		zdiv(_a, _a, _b);
	}
	return size_total + (zsignum(a) < 0);
}

static void
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

static void
zbset(z_t r, z_t a, size_t bit, int mode)
{
	zrsh(_a, a, bit);
	if (mode && zeven(_a)) {
		zlsh(_a, _1, bit);
		zadd(r, a, _a);
	} else if (mode <= 0 && zodd(_a)) {
		zlsh(_a, _1, bit);
		zsub(r, a, _a);
	} else {
		zset(r, a);
	}
}

static void
zrand(z_t r, int dev, int dist, z_t n)
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

static void
zand(z_t r, z_t a, z_t b)
{
	int neg = hebi_sign(a) < 0 && hebi_sign(b) < 0;
	hebi_uword x;
	size_t i = 0;
	zset(_a, a);
	zset(_b, b);
	zsetu(r, 0);
	while (zsignum(_a) && zsignum(_b)) {
		x = hebi_get_u(_a) & hebi_get_u(_b);
		zsetu(_m, x);
		zlsh(_m, _m, i * 8 * sizeof(x));
		zadd(r, r, _m);
		zrsh(_a, _a, 8 * sizeof(x));
		zrsh(_b, _b, 8 * sizeof(x));
	}
	if (neg)
		zneg(r, r);
}

static void
zor(z_t r, z_t a, z_t b)
{
	int neg = hebi_sign(a) < 0 || hebi_sign(b) < 0;
	hebi_uword x;
	size_t i = 0;
	zset(_a, a);
	zset(_b, b);
	zsetu(r, 0);
	while (zsignum(_a) || zsignum(_b)) {
		x = hebi_get_u(_a) | hebi_get_u(_b);
		zsetu(_m, x);
		zlsh(_m, _m, i * 8 * sizeof(x));
		zadd(r, r, _m);
		zrsh(_a, _a, 8 * sizeof(x));
		zrsh(_b, _b, 8 * sizeof(x));
	}
	if (neg)
		zneg(r, r);
}

static void
zxor(z_t r, z_t a, z_t b)
{
	int neg = (hebi_sign(a) < 0) ^ (hebi_sign(b) < 0);
	hebi_uword x;
	size_t i = 0;
	zset(_a, a);
	zset(_b, b);
	zsetu(r, 0);
	while (zsignum(_a) || zsignum(_b)) {
		x = hebi_get_u(_a) ^ hebi_get_u(_b);
		zsetu(_m, x);
		zlsh(_m, _m, i * 8 * sizeof(x));
		zadd(r, r, _m);
		zrsh(_a, _a, 8 * sizeof(x));
		zrsh(_b, _b, 8 * sizeof(x));
	}
	if (neg)
		zneg(r, r);
}

void
zgcd(z_t r, z_t a, z_t b)
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
	zrsh(_a, a, a_lsb);
	zrsh(_b, b, b_lsb);

	for (;;) {
		if ((cmpmag = zcmpmag(_a, _b)) >= 0) {
			if (cmpmag == 0)
				break;
			zswap(_a, _b);
		}
		zsub(_b, _b, _a);
		zrsh(_b, _b, zlsb(_b));
	}

	zlsh(r, _a, shifts);
	if (neg)
		zneg(r, r);
}

static void
znot(z_t r, z_t a)
{
	size_t bits = zbits(a);
	zsetu(_b, 0);
	zsetu(_a, 1);
	zlsh(_a, _a, bits);
	zadd(_b, _b, _a);
	zsub(_b, _b, _1);
	zxor(r, a, _b);
	zneg(r, r);
}

int
hebi_shl(hebi_int *r, const hebi_int *a, unsigned int b)
{
	zsetu(_a, 2);
	zpowu(_a, _a, b);
	zmul(r, a, _a);
	return hebi_success;
}

int
hebi_shr(hebi_int *r, const hebi_int *a, unsigned int b)
{
	zsetu(_a, 2);
	zpowu(_a, _a, b);
	zdiv(r, a, _a);
	return hebi_success;
}

int
hebi_div(hebi_int *r, hebi_int *m, const hebi_int *a, const hebi_int *b)
{
	/* TODO */
	return hebi_success;
}
