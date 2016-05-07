#include <tfm.h>

#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BIGINT_LIBRARY "TomsFastMath"

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

typedef fp_int z_t[1];

static z_t _0, _1, _a, _b;
static int _tmp, error;
static jmp_buf jbuf;

#ifdef ZAHL_UNSAFE
# define try(expr) (expr)
#else
# define try(expr) do if ((error = (expr))) longjmp(jbuf, 1); while (0)
#endif

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

static inline void
zperror(const char *str)
{
	const char *serr;
	switch (error) {
	case FP_VAL: serr = "FP_VAL";        break;
	case FP_MEM: serr = "FP_MEM";        break;
	default:     serr = "unknown error"; break;
	}
	if (str && *str)
		fprintf(stderr, "%s: %s\n", str, serr);
	else
		fprintf(stderr, "%s\n", serr);
}

static inline void
zinit(z_t a)
{
	fp_init(a);
}

static inline void
zfree(z_t a)
{
	(void) a;
}

static inline void
zadd(z_t r, z_t a, z_t b)
{
	fp_add(a, b, r);
}

static inline void
zsub(z_t r, z_t a, z_t b)
{
	fp_sub(a, b, r);
}

static inline void
zset(z_t r, z_t a)
{
	fp_copy(a, r);
}

static inline int
zeven(z_t a)
{
	return fp_iseven(a);
}

static inline int
zodd(z_t a)
{
	return fp_isodd(a);
}

static inline int
zeven_nonzero(z_t a)
{
	return fp_iseven(a);
}

static inline int
zodd_nonzero(z_t a)
{
	return fp_isodd(a);
}

static inline int
zzero(z_t a)
{
	return fp_iszero(a);
}

static inline int
zsignum(z_t a)
{
	return fp_cmp(a, _0);
}

static inline size_t
zbits(z_t a)
{
	return fp_count_bits(a);
}

static inline size_t
zlsb(z_t a)
{
	return fp_cnt_lsb(a);
}

static inline void
zlsh(z_t r, z_t a, size_t b)
{
	fp_mul_2d(a, b, r);
}

static inline void
zrsh(z_t r, z_t a, size_t b)
{
	fp_div_2d(a, b, r, 0);
}

static inline void
ztrunc(z_t r, z_t a, size_t b)
{
	fp_mod_2d(a, b, r);
}

static inline void
zgcd(z_t r, z_t a, z_t b)
{
	fp_gcd(a, b, r);
}

static inline void
zmul(z_t r, z_t a, z_t b)
{
	fp_mul(a, b, r);
}

static inline void
zsqr(z_t r, z_t a)
{
	fp_sqr(a, r);
}

static inline void
zmodmul(z_t r, z_t a, z_t b, z_t m)
{
	try(fp_mulmod(a, b, m, r));
}

static inline void
zmodsqr(z_t r, z_t a, z_t m)
{
	try(fp_sqrmod(a, m, r));
}

static inline void
zsets(z_t a, char *s)
{
	try(fp_read_radix(a, s, 10));
}

static inline size_t
zstr_length(z_t a, unsigned long long int b)
{
	try(fp_radix_size(a, b, &_tmp));
	return _tmp;
}

static inline char *
zstr(z_t a, char *s, size_t n)
{
	try(fp_toradix(a, s, 10));
	return s;
	(void) n;
}

static inline int
zptest(z_t w, z_t a, int t)
{
	return fp_isprime_ex(a, t);
	(void) w; /* Note, the witness is not returned. */
}

static inline void
zdivmod(z_t q, z_t r, z_t a, z_t b)
{
	try(fp_div(a, b, q, r));
}

static inline void
zdiv(z_t q, z_t a, z_t b)
{
	zdivmod(q, 0, a, b);
}

static inline void
zmod(z_t r, z_t a, z_t b)
{
	try(fp_mod(a, b, r));
}

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

static inline void
zswap(z_t a, z_t b)
{
	z_t t;
	zset(t, a);
	zset(a, b);
	zset(b, t);
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

static inline size_t
zsave(z_t a, char *s)
{
	_tmp = !s ? fp_signed_bin_size(a) : (fp_to_signed_bin(a, (unsigned char *)s), 0);
	return _tmp;
}

static inline size_t
zload(z_t a, const char *s)
{
	fp_read_signed_bin(a, (unsigned char *)s, _tmp);
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

static inline int
zcmpmag(z_t a, z_t b)
{
	return fp_cmp_mag(a, b);
}

static inline int
zcmp(z_t a, z_t b)
{
	return fp_cmp(a, b);
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
	fp_2expt(_a, zbits(a));
	fp_sub_d(_a, 1, _a);
	zand(r, a, _a);
	zneg(r, r);
}

static inline int
zbtest(z_t a, size_t bit)
{
	fp_2expt(_b, bit);
	zand(_b, a, _b);
	return !zzero(_b);
}

static inline void
zbset(z_t r, z_t a, size_t bit, int mode)
{
	if (mode > 0) {
		fp_2expt(_b, bit);
		zor(r, a, _b);
	} else if (mode < 0 || zbtest(a, bit)) {
		fp_2expt(_b, bit);
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

static inline void
zpow(z_t r, z_t a, z_t b)
{
	zpowu(r, a, b->used ? b->dp[0] : 0);
}

static inline void
zmodpow(z_t r, z_t a, z_t b, z_t m)
{
	try(fp_exptmod(a, b, m, r));
}

static inline void
zmodpowu(z_t r, z_t a, unsigned long long int b, z_t m)
{
	fp_set_int(_b, b);
	zmodpow(r, a, _b, m);
}
