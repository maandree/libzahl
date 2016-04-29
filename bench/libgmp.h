#define __GMP_NO_ATTRIBUTE_CONST_PURE

#include <gmp.h>

#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BIGINT_LIBRARY "GMP"

typedef mpz_t z_t;

static z_t _0, _1, _a, _b;
static FILE *_fbuf;
static gmp_randstate_t _randstate;

static inline void
zsetup(jmp_buf env)
{
	static char buf[1000];
	(void) env;
	mpz_init_set_ui(_0, 0);
	mpz_init_set_ui(_1, 1);
	mpz_init(_a);
	mpz_init(_b);
	_fbuf = fmemopen(buf, sizeof(buf), "r+");
	gmp_randinit_mt(_randstate);
}

static inline void
zunsetup(void)
{
	mpz_clear(_0);
	mpz_clear(_1);
	mpz_clear(_a);
	mpz_clear(_b);
	fclose(_fbuf);
	gmp_randclear(_randstate);
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
#define zinit                   mpz_init
#define zfree                   mpz_clear

#define zset                    mpz_set
#define zneg                    mpz_neg
#define zabs                    mpz_abs
#define zadd_unsigned(r, a, b)  (zabs(_a, a), zabs(_b, b), mpz_add(r, _a, _b))
#define zsub_unsigned(r, a, b)  (zabs(_a, a), zabs(_b, b), mpz_sub(r, _a, _b))
#define zadd                    mpz_add
#define zsub                    mpz_sub
#define zand                    mpz_and
#define zor                     mpz_ior
#define zxor                    mpz_xor
#define zbtest                  mpz_tstbit
#define zeven_nonzero           zeven
#define zodd_nonzero            zodd
#define zzero(a)                (!mpz_sgn(a))
#define zsignum                 mpz_sgn
#define zbits(a)                mpz_sizeinbase(a, 2)
#define zlsb(a)                 mpz_scan1(a, 0)
#define zswap                   mpz_swap
#define zlsh                    mpz_mul_2exp
#define zrsh                    mpz_tdiv_q_2exp
#define ztrunc                  mpz_tdiv_r_2exp
#define zcmpmag                 mpz_cmpabs
#define zcmp                    mpz_cmp
#define zcmpi(a, b)             (zseti(_b, b), zcmp(a, _b))
#define zcmpu(a, b)             (zsetu(_b, b), zcmp(a, _b))
#define zgcd                    mpz_gcd
#define zmul                    mpz_mul
#define zsqr(r, a)              mpz_mul(r, a, a)
#define zmodmul(r, a, b, m)     (zmul(r, a, b), zmod(r, r, m))
#define zmodsqr(r, a, m)        (zsqr(r, a), zmod(r, r, m))
#define zpow(r, a, b)           mpz_pow_ui(r, a, mpz_get_ui(b))
#define zpowu                   mpz_pow_ui
#define zmodpow                 mpz_powm
#define zmodpowu                mpz_powm_ui
#define zsets(a, s)             mpz_set_str(a, s, 10)
#define zstr_length(a, b)       (mpz_sizeinbase(a, 10) + (zsignum(a) < 0))
#define zstr(a, s, n)           (mpz_get_str(s, 10, a))
#define zptest(w, a, t)         mpz_probab_prime_p(a, t) /* Note, the witness is not returned. */
#define zdiv                    mpz_tdiv_q
#define zmod                    mpz_tdiv_r
#define zdivmod                 mpz_tdiv_qr

static inline int
zeven(z_t a)
{
	return mpz_even_p(a);
}

static inline int
zodd(z_t a)
{
	return mpz_odd_p(a);
}

static inline void
zsetu(z_t r, unsigned long long int val)
{
	uint32_t high = (uint32_t)(val >> 32);
	uint32_t low = (uint32_t)val;

	if (high) {
		mpz_set_ui(r, high);
		mpz_set_ui(_a, low);
		zlsh(r, r, 32);
		zadd(r, r, _a);
	} else {
		mpz_set_ui(r, low);
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
znot(z_t r, z_t a)
{
	size_t bits = zbits(a);
	mpz_set_ui(_b, 0);
	mpz_setbit(_b, bits);
	zsub(_b, _b, _1);
	zxor(r, a, _b);
	zneg(r, r);
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
zbset(z_t r, z_t a, size_t bit, int mode)
{
	if (r != a)
		zset(r, a);
	if (mode > 0)
		mpz_setbit(r, bit);
	else if (mode == 0)
		mpz_clrbit(r, bit);
	else
		mpz_combit(r, bit);
}

static inline size_t
zsave(z_t a, void *buffer)
{
	size_t n = mpz_out_raw(_fbuf, a);
	(void)buffer;
	fseek(_fbuf, -(long)n, SEEK_CUR);
	return n;
}

static inline size_t
zload(z_t a, const void *buffer)
{
	size_t n = mpz_inp_raw(a, _fbuf);
	(void)buffer;
	fseek(_fbuf, -(long)n, SEEK_CUR);
	return n;
}

static inline void
zrand(z_t r, int dev, int dist, z_t n)
{
	size_t bits;
	(void) dev;

	if (zzero(n)) {
		mpz_set_ui(r, 0);
		return;
	}
	if (zsignum(n) < 0) {
		return;
	}

	switch (dist) {
	case QUASIUNIFORM:
		bits = zbits(n);
		mpz_urandomb(r, _randstate, bits);
		zadd(r, r, _1);
		zmul(r, r, n);
		zrsh(r, r, bits);
		break;

	case UNIFORM: /* Note, n is exclusive in this implementation. */
		mpz_urandomm(r, _randstate, n);
		break;

	case MODUNIFORM:
		bits = zbits(n);
		mpz_urandomb(r, _randstate, bits);
		if (zcmp(r, n) > 0)
			zsub(r, r, n);
		break;

	default:
		abort();
	}
}
