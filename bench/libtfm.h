#include <tfm.h>

#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>

#define BIGINT_LIBRARY "TomsFastMath"

typedef fp_int z_t[1];

static z_t _0, _1, _a, _b;
static int _tmp, error;
static jmp_buf jbuf;

static inline void
fp_set_int(z_t a, uint32_t d)
{
	a->dp[0] = d;
	a->used = !!d;
	a->sign = 0;
}

static inline void
zsetup(jmp_buf env)
{
	*jbuf = *env;
	fp_init(_0);
	fp_init(_1);
	fp_init(_a);
	fp_init(_b);
	fp_set_int(_0, 0);
	fp_set_int(_1, 1);
}

static inline void
zunsetup(void)
{
	/* Do nothing */
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
#define zinit(a)                fp_init(a)
#define zfree(a)                ((void)a)

#define zset(r, a)              fp_copy(a, r)
#define zadd_unsigned(r, a, b)  (zabs(_a, a), zabs(_b, b), fp_add(_a, _b, r))
#define zsub_unsigned(r, a, b)  (zabs(_a, a), zabs(_b, b), fp_sub(_a, _b, r))
#define zadd(r, a, b)           fp_add(a, b, r)
#define zsub(r, a, b)           fp_sub(a, b, r)
#define zeven(a)                fp_iseven(a)
#define zodd(a)                 fp_isodd(a)
#define zeven_nonzero(a)        fp_iseven(a)
#define zodd_nonzero(a)         fp_isodd(a)
#define zzero(a)                fp_iszero(a)
#define zsignum(a)              fp_cmp(a, _0)
#define zbits(a)                fp_count_bits(a)
#define zlsb(a)                 fp_cnt_lsb(a)
#define zlsh(r, a, b)           fp_mul_2d(a, b, r)
#define zrsh(r, a, b)           fp_div_2d(a, b, r, 0)
#define ztrunc(r, a, b)         fp_mod_2d(a, b, r)
#define zcmpmag(a, b)           fp_cmp_mag(a, b)
#define zcmp(a, b)              fp_cmp(a, b)
#define zcmpi(a, b)             (zseti(_b, b), fp_cmp(a, _b))
#define zcmpu(a, b)             (zsetu(_b, b), fp_cmp(a, _b))
#define zgcd(r, a, b)           fp_gcd(a, b, r)
#define zmul(r, a, b)           fp_mul(a, b, r)
#define zsqr(r, a)              fp_sqr(a, r)
#define zmodmul(r, a, b, m)     fp_mulmod(a, b, m, r)
#define zmodsqr(r, a, m)        fp_sqrmod(a, m, r)
#define zpow(r, a, b)           zpowu(r, a, b->used ? b->dp[0] : 0)
#define zmodpow(r, a, b, m)     fp_exptmod(a, b, m, r)
#define zmodpowu(r, a, b, m)    (fp_set_int(_b, b), fp_exptmod(a, _b, m, r))
#define zsets(a, s)             fp_read_radix(a, s, 10)
#define zstr_length(a, b)       (fp_radix_size(a, b, &_tmp), _tmp)
#define zstr(a, s, n)           (fp_toradix(a, s, 10))
#define zptest(w, a, t)         fp_isprime_ex(a, t) /* Note, the witness is not returned. */
#define zload(a, s)             fp_read_signed_bin(a, (unsigned char *)s, _tmp)
#define zdiv(r, a, b)           fp_div(a, b, r, 0)
#define zmod(r, a, b)           fp_mod(a, b, r)
#define zdivmod(q, r, a, b)     fp_div(a, b, q, r)

static inline void
zneg(z_t r, z_t a)
{
	fp_neg(a, r);
}

static inline void
zabs(z_t r, z_t a)
{
	fp_abs(a, r);
}

static inline void
zswap(z_t a, z_t b)
{
	z_t t;
	fp_copy(a, t);
	fp_copy(b, a);
	fp_copy(t, b);
}

static inline void
zand(z_t r, z_t a, z_t b)
{
	int i;
	for (i = 0; i < a->used && i < b->used; i++)
		r->dp[i] = a->dp[i] & b->dp[i];
	r->used = i;
	while (r->used && !r->dp[r->used])
		r->used--;
	r->sign = a->sign & b->sign;
}

static inline void
zor(z_t r, z_t a, z_t b)
{
	int i;
	for (i = 0; i < a->used && i < b->used; i++)
		r->dp[i] = a->dp[i] | b->dp[i];
	for (; i < a->used; i++)
		r->dp[i] = a->dp[i];
	for (; i < b->used; i++)
		r->dp[i] = b->dp[i];
	r->used = i;
	r->sign = a->sign | b->sign;
}

static inline void
zxor(z_t r, z_t a, z_t b)
{
	int i;
	for (i = 0; i < a->used && i < b->used; i++)
		r->dp[i] = a->dp[i] ^ b->dp[i];
	for (; i < a->used; i++)
		r->dp[i] = a->dp[i];
	for (; i < b->used; i++)
		r->dp[i] = b->dp[i];
	r->used = i;
	while (r->used && !r->dp[r->used])
		r->used--;
	r->sign = a->sign ^ b->sign;
}

static inline int
zsave(z_t a, char *buf)
{
	_tmp = buf ? fp_signed_bin_size(a) : (fp_to_signed_bin(a, (unsigned char *)buf), 0);
	return _tmp;
}

static inline void
zsetu(z_t r, unsigned long long int val)
{
	uint32_t high = (uint32_t)(val >> 32);
	uint32_t low = (uint32_t)val;

	if (high) {
		fp_set_int(r, high);
		fp_set_int(_a, low);
		fp_lshd(r, 32);
		zadd(r, r, _a);
	} else {
		fp_set_int(r, low);
	}
	
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
znot(z_t r, z_t a)
{
	fp_2expt(_a, (int)zbits(a));
	fp_sub_d(_a, 1, _a);
	zand(r, a, _a);
	zneg(r, r);
}

static inline int
zbtest(z_t a, size_t bit)
{
	fp_2expt(_b, (int)bit);
	zand(_b, a, _b);
	return !zzero(_b);
}

static inline void
zbset(z_t r, z_t a, size_t bit, int mode)
{
	if (mode > 0) {
		fp_2expt(_b, (int)bit);
		zor(r, a, _b);
	} else if (mode < 0 || zbtest(a, bit)) {
		fp_2expt(_b, (int)bit);
		zxor(r, a, _b);
	}
}

static inline void
zrand(z_t r, int dev, int dist, z_t n)
{
	static int gave_up = 0;
	int bits;
	(void) dev;

	if (zzero(n)) {
		fp_zero(r);
		return;
	}
	if (zsignum(n) < 0) {
		return;
	}

	bits = zbits(n);

	switch (dist) {
	case QUASIUNIFORM:
		fp_rand(r, bits);
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
		fp_rand(r, bits);
		if (zcmp(r, n) > 0)
			zsub(r, r, n);
		break;

	default:
		abort();
	}
}

static inline void
zpowu(z_t r, z_t a, unsigned long long int b)
{
	if (!b) {
		if (zzero(a)) {
			error = FP_VAL;
			longjmp(jbuf, 1);
		}
		zsetu(r, 1);
		return;
	}
	zset(_a, a);
	zsetu(r, 1);
	for (; b; b >>= 1) {
		if (b & 1)
			zmul(r, r, _a);
		zsqr(_a, _a);
	}
}
