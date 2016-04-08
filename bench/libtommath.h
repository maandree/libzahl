#include <tommath.h>

#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>

typedef mp_int z_t[1];

static z_t _0, _1, _a, _b;
static int _tmp;

static void
zsetup(jmp_buf env)
{
	(void) env;
	mp_init_set_int(_0, 0);
	mp_init_set_int(_1, 1);
	mp_init(_a);
	mp_init(_b);
}

static void
zunsetup(void)
{
	mp_clear(_0);
	mp_clear(_1);
	mp_clear(_a);
	mp_clear(_b);
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
#define zinit(a)                mp_init(a)
#define zfree(a)                mp_clear(a)

#define zset(r, a)              mp_copy(a, r)
#define zneg(r, a)              mp_neg(a, r)
#define zabs(r, a)              mp_abs(a, r)
#define zadd_unsigned(r, a, b)  (mp_abs(a, _a), mp_abs(b, _b), mp_add(_a, _b, r))
#define zsub_unsigned(r, a, b)  (mp_abs(a, _a), mp_abs(b, _b), mp_sub(_a, _b, r))
#define zadd(r, a, b)           mp_add(a, b, r)
#define zsub(r, a, b)           mp_sub(a, b, r)
#define zand(r, a, b)           mp_and(a, b, r)
#define zor(r, a, b)            mp_or(a, b, r)
#define zxor(r, a, b)           mp_xor(a, b, r)
#define zeven(a)                mp_iseven(a)
#define zodd(a)                 mp_isodd(a)
#define zeven_nonzero(a)        mp_iseven(a)
#define zodd_nonzero(a)         mp_isodd(a)
#define zzero(a)                mp_iszero(a)
#define zsignum(a)              mp_cmp(a, _0)
#define zbits(a)                mp_count_bits(a)
#define zlsb(a)                 mp_cnt_lsb(a)
#define zswap(a, b)             mp_exch(a, b)
#define zlsh(r, a, b)           mp_mul_2d(a, b, r)
#define zrsh(r, a, b)           mp_div_2d(a, b, r, 0)
#define ztrunc(r, a, b)         mp_mod_2d(a, b, r)
#define zcmpmag(a, b)           mp_cmp_mag(a, b)
#define zcmp(a, b)              mp_cmp(a, b)
#define zcmpi(a, b)             (zseti(_b, b), mp_cmp(a, _b))
#define zcmpu(a, b)             (zsetu(_b, b), mp_cmp(a, _b))
#define zgcd(r, a, b)           mp_gcd(a, b, r)
#define zmul(r, a, b)           mp_mul(a, b, r)
#define zsqr(r, a)              mp_sqr(a, r)
#define zmodmul(r, a, b, m)     mp_mulmod(a, b, m, r)
#define zmodsqr(r, a, m)        mp_sqrmod(a, m, r)
#define zpow(r, a, b)           mp_expt_d(a, (mp_digit)mp_get_int(b), r)
#define zpowu(r, a, b)          mp_expt_d(a, b, r)
#define zmodpow(r, a, b, m)     mp_exptmod(a, b, m, r)
#define zmodpowu(r, a, b, m)    (mp_set_int(_b, b), mp_exptmod(a, _b, m, r))
#define zsets(a, s)             mp_read_radix(a, s, 10)
#define zstr_length(a, b)       (mp_radix_size(a, b, &_tmp), _tmp)
#define zstr(a, s)              mp_toradix(a, s, 10)
#define zptest(w, a, t)         (mp_prime_is_prime(a, t, &_tmp), _tmp) /* Note, the witness is not returned. */
#define zload(a, s)             mp_read_signed_bin(a, (unsigned char *)s, _tmp)
#define zdiv(r, a, b)           mp_div(a, b, r, 0)
#define zmod(r, a, b)           mp_mod(a, b, r)
#define zdivmod(q, r, a, b)     mp_div(a, b, q, r)

static int
zsave(z_t a, char *buf)
{
	_tmp = buf ? mp_signed_bin_size(a) : mp_to_signed_bin(a, (unsigned char *)buf);
	return _tmp;
}

static void
zsetu(z_t r, unsigned long long int val)
{
	uint32_t high = (uint32_t)(val >> 32);
	uint32_t low = (uint32_t)val;

	if (high) {
		mp_set_int(r, high);
		mp_set_int(_a, low);
		mp_lshd(r, 32);
		zadd(r, r, _a);
	} else {
		mp_set_int(r, low);
	}
	
}

static void
zseti(z_t r, long long int val)
{
	if (val >= 0) {
		zsetu(r, (unsigned long long int)val);
	} else {
		zsetu(r, (unsigned long long int)-val);
		zneg(r, r);
	}
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
znot(z_t r, z_t a)
{
	mp_2expt(_a, (int)zbits(a));
	mp_sub_d(_a, 1, _a);
	zand(r, a, _a);
	zneg(r, r);
}

static int
zbtest(z_t a, size_t bit)
{
	mp_2expt(_b, (int)bit);
	zand(_b, a, _b);
	return !zzero(_b);
}

static void
zbset(z_t r, z_t a, size_t bit, int mode)
{
	if (mode > 0) {
		mp_2expt(_b, (int)bit);
		zor(r, a, _b);
	} else if (mode < 0 || zbtest(a, bit)) {
		mp_2expt(_b, (int)bit);
		zxor(r, a, _b);
	}
}

static void
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
		mp_rand(r, bits);
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
		mp_rand(r, bits);
		if (zcmp(r, n) > 0)
			zsub(r, r, n);
		break;

	default:
		abort();
	}
}
