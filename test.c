/* See LICENSE file for copyright and license details. */
#include "zahl.h"

#include <stdio.h>
#include <string.h>

#define assert(expr, expected)\
	do {\
		int got = (expr);\
		if (!(got expected)) {\
			fprintf(stderr,\
				"Failure at line %i: %s, expected %s, but got %i.\n",\
				__LINE__, #expr, #expected, got);\
			return 1;\
		}\
	} while (0)

#define assert_zu(expr, expected)\
	do {\
		size_t got = (expr);\
		if (got != (expected)) {\
			fprintf(stderr,\
				"Failure at line %i: %s, expected %zu, but got %zu.\n",\
				__LINE__, #expr, (size_t)(expected), got);\
			return 1;\
		}\
	} while (0)

#define assert_s(expr, expected)\
	do {\
		const char *got = (expr);\
		if (strcmp(got, expected)) {\
			fprintf(stderr,\
				"Failure at line %i: %s, expected %s, but got %s.\n",\
				__LINE__, #expr, expected, got);\
			return 1;\
		}\
	} while (0)

int
main(void)
{
	/* static because otherwise it would have to be volatile yeilding a lot of stupid
	 * warnings. auto variables are not guaranteed to be readable after a long jump. */
	static z_t a, b, c, d, _0, _1, _2, _3;
	
	jmp_buf env;
	char buf[1000];
	int ret = 0;
	size_t n;

	if (setjmp(env)) {
		ret = 1;
		goto done;
	}

	zsetup(env);
	zinit(a), zinit(b), zinit(c), zinit(d), zinit(_0), zinit(_1), zinit(_2), zinit(_3);

	zsetu(_0, 0);
	zsetu(_1, 1);
	zsetu(_2, 2);
	zsetu(_3, 3);

	assert(zeven(_0), == 1);
	assert(zodd(_0), == 0);
	assert(zzero(_0), == 1);
	assert(zsignum(_0), == 0);
	assert(zeven(_1), == 0);
	assert(zodd(_1), == 1);
	assert(zzero(_1), == 0);
	assert(zsignum(_1), == 1);
	assert(zeven(_2), == 1);
	assert(zodd(_2), == 0);
	assert(zzero(_2), == 0);
	assert(zsignum(_2), == 1);

	zswap(_1, _2);
	assert(zeven(_2), == 0);
	assert(zodd(_2), == 1);
	assert(zzero(_2), == 0);
	assert(zsignum(_2), == 1);
	assert(zeven(_1), == 1);
	assert(zodd(_1), == 0);
	assert(zzero(_1), == 0);
	assert(zsignum(_1), == 1);
	zswap(_2, _1);
	assert(zeven(_1), == 0);
	assert(zodd(_1), == 1);
	assert(zzero(_1), == 0);
	assert(zsignum(_1), == 1);
	assert(zeven(_2), == 1);
	assert(zodd(_2), == 0);
	assert(zzero(_2), == 0);
	assert(zsignum(_2), == 1);
	
	assert((zneg(_2, _2), zsignum(_2)), == -1); zneg(_2, _2);
	assert(zsignum(_2), == 1);

	assert(zcmp(_0, _0), == 0);
	assert(zcmp(_1, _1), == 0);
	assert(zcmp(_0, _1), < 0);
	assert(zcmp(_1, _0), > 0);
	assert(zcmp(_1, _2), < 0);
	assert(zcmp(_2, _1), > 0);
	assert(zcmp(_0, _2), < 0);
	assert(zcmp(_2, _0), > 0);

	zbset(a, _0, 0, 1);
	assert(zcmp(a, _1), == 0);
	zbset(a, a, 1, 1);
	assert(zcmp(a, _3), == 0);
	zbset(a, a, 0, 0);
	assert(zcmp(a, _2), == 0);
	zbset(a, a, 0, 0);
	assert(zcmp(a, _2), == 0);
	zbset(a, a, 0, -1);
	assert(zcmp(a, _3), == 0);
	zbset(a, a, 0, -1);
	assert(zcmp(a, _2), == 0);

	zadd(a, _0, _1);
	assert(zsignum(a), == 1);
	assert(zcmp(a, _1), == 0);
	assert(zcmpi(a, 1), == 0);
	assert(zcmpu(a, 1), == 0);
	zneg(a, a);
	assert(zsignum(a), == -1);
	assert(zcmp(a, _1), < 0);
	assert(zcmpi(a, 1), < 0);
	assert(zcmpu(a, 1), < 0);
	zadd(a, _2, _0);
	assert(zsignum(a), == 1);
	assert(zcmp(a, _2), == 0);
	assert(zcmpi(a, 2), == 0);
	assert(zcmpu(a, 2), == 0);
	zneg(a, a);
	assert(zsignum(a), == -1);
	assert(zcmp(a, _2), < 0);
	assert(zcmpi(a, 2), < 0);
	assert(zcmpu(a, 2), < 0);
	assert(zsignum(_1), == 1);
	zadd(a, _1, _1);
	assert(zsignum(a), == 1);
	assert(zcmp(a, _2), == 0);
	assert(zcmpi(a, 2), == 0);
	assert(zcmpu(a, 2), == 0);
	zset(b, _1);
	zadd(a, b, _1);
	assert(zsignum(a), == 1);
	assert(zcmp(a, _2), == 0);
	assert(zcmpi(a, 2), == 0);
	assert(zcmpu(a, 2), == 0);
	zneg(a, a);
	zset(b, _2);
	zneg(b, b);
	assert(zsignum(a), == -1);
	assert(zcmp(a, b), == 0);
	assert(zcmp(a, _2), < 0);
	assert(zcmpmag(a, b), == 0);
	assert(zcmpmag(a, _2), == 0);
	assert(zcmpi(a, 2), < 0);
	assert(zcmpu(a, 2), < 0);
	assert(zcmpi(a, -2), == 0);
	assert((zneg(_2, _2), zcmp(a, _2)), == 0); zneg(_2, _2);
	zadd(a, _1, _2);
	assert(zsignum(a), == 1);
	assert(zcmp(a, _2), > 0);
	assert(zcmpi(a, 2), > 0);
	assert(zcmpu(a, 2), > 0);
	zneg(a, a);
	zset(b, _2);
	zneg(b, b);
	assert(zsignum(a), == -1);
	assert(zcmpmag(a, _2), > 0);
	assert(zcmpmag(a, b), > 0);
	assert(zcmp(a, b), < 0);
	assert(zcmp(a, _2), < 0);
	assert(zcmpi(a, 2), < 0);
	assert(zcmpu(a, 2), < 0);
	assert(zcmpi(a, -2), < 0);
	assert((zneg(_2, _2), zcmp(a, _2)), < 0); zneg(_2, _2);
	zneg(b, _3);
	assert(zcmp(a, b), == 0);

	zsub(a, _2, _1);
	assert(zcmpmag(_2, _1), > 0);
	assert(zcmpmag(_2, _0), > 0);
	assert(zcmpmag(_1, _0), > 0);
	zsub(b, _1, _2);
	assert(zcmpmag(_2, _0), > 0);
	assert(zcmpmag(_1, _0), > 0);
	assert(zcmpmag(_2, _1), > 0);
	assert(zcmpmag(a, b), == 0);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, b), > 0);
	assert(zcmp(a, _1), == 0);
	assert(zcmp(b, _1), < 0);
	zsub(a, _1, _1);
	assert(zcmp(a, _0), == 0);
	zseti(b, 0);
	zsetu(c, 0);
	zsub(a, b, c);
	assert(zcmp(a, _0), == 0);
	assert(zcmpmag(_2, _1), > 0);
	assert(zcmp(_2, _1), > 0);
	zsub(a, _2, _1);
	assert(zsignum(a), == 1);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, _1), == 0);
	zsub(a, a, _1);
	assert(zcmp(a, _0), == 0);
	zsub(a, a, _0);
	assert(zcmp(a, _0), == 0);
	zsub(a, _1, _2);
	assert(zcmp(a, _1), < 0);
	assert(zcmpmag(a, _1), == 0);
	zabs(a, a);
	assert(zcmp(a, _1), == 0);
	zabs(a, a);
	assert(zcmp(a, _1), == 0);
	zabs(a, _1);
	assert(zcmp(a, _1), == 0);
	zabs(a, _0);
	assert(zcmp(a, _0), == 0);

	zseti(b, -1);
	zseti(c, -2);
	zadd(a, _0, b);
	assert(zcmp(a, _0), < 0);
	assert(zcmpi(a, -1), == 0);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, _1), < 0);
	zadd(a, b, _0);
	assert(zcmp(a, _0), < 0);
	assert(zcmpi(a, -1), == 0);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, _1), < 0);
	zadd(a, b, c);
	assert(zcmp(a, c), < 0);
	assert(zcmpmag(a, _2), > 0);
	zadd(a, c, b);
	assert(zcmp(a, c), < 0);
	assert(zcmpmag(a, _2), > 0);
	zadd(a, b, _1);
	assert(zcmp(a, _0), == 0);
	assert(zcmpmag(a, _0), == 0);
	zadd(a, _1, b);
	assert(zcmp(a, _0), == 0);
	assert(zcmpmag(a, _0), == 0);

	zneg(b, _1);
	zneg(c, _2);
	zsub(a, _0, b);
	assert(zcmp(a, _1), == 0);
	zsub(a, b, _0);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, _1), < 0);
	zsub(a, b, c);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, _1), == 0);
	zsub(a, c, b);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, _1), < 0);
	zsub(a, b, _1);
	assert(zcmpmag(a, _2), == 0);
	assert(zcmp(a, _2), < 0);
	assert(zcmp(a, c), == 0);
	zsub(a, _1, b);
	assert(zcmp(b, _1), < 0);
	assert(zcmpmag(b, _1), == 0);
	assert(zcmp(a, _2), == 0);

	zsetu(a, 1000);
	zsetu(b, 0);
	assert(zcmp(a, b), != 0);
	n = zsave(a, buf);
	assert(n, > 0);
	assert_zu(zload(b, buf), n);
	assert(zcmp(a, b), == 0);

	zneg(b, _1);
	zneg(c, _2);

	assert((zadd_unsigned(a, b, c), zcmp(a, _3)), == 0);
	assert((zadd_unsigned(a, b, c), zcmp(a, _3)), == 0);
	assert((zadd_unsigned(a, b, _2), zcmp(a, _3)), == 0);
	assert((zadd_unsigned(a, _1, c), zcmp(a, _3)), == 0);

	assert((zsub_unsigned(a, _2, _1), zcmp(a, _1)), == 0);
	assert((zsub_unsigned(a, _2, b), zcmp(a, _1)), == 0);
	assert((zsub_unsigned(a, c, _1), zcmp(a, _1)), == 0);
	assert((zsub_unsigned(a, c, b), zcmp(a, _1)), == 0);

	assert((zsub_unsigned(a, _1, _2), zcmp(a, b)), == 0);
	assert((zsub_unsigned(a, b, _2), zcmp(a, b)), == 0);
	assert((zsub_unsigned(a, _1, c), zcmp(a, b)), == 0);
	assert((zsub_unsigned(a, b, c), zcmp(a, b)), == 0);

	assert_zu(zbits(_0), 1);
	assert_zu(zbits(_1), 1);
	assert_zu(zbits(_2), 2);
	assert_zu(zbits(_3), 2);

	assert_zu(zlsb(_0), SIZE_MAX);
	assert_zu(zlsb(_1), 0);
	assert_zu(zlsb(_2), 1);
	assert_zu(zlsb(_3), 0);

	assert((zand(a, _0, _0), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zand(a, _0, _1), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zand(a, _0, _2), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zand(a, _0, _3), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zand(a, _1, _1), zcmp(a, _1)), == 0);
	assert((zand(a, _1, _2), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zand(a, _1, _3), zcmp(a, _1)), == 0);
	assert((zand(a, _2, _2), zcmp(a, _2)), == 0);
	assert((zand(a, _2, _3), zcmp(a, _2)), == 0);
	assert((zand(a, _3, _3), zcmp(a, _3)), == 0);

	assert((zor(a, _0, _0), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zor(a, _0, _1), zcmp(a, _1)), == 0);
	assert((zor(a, _0, _2), zcmp(a, _2)), == 0);
	assert((zor(a, _0, _3), zcmp(a, _3)), == 0);
	assert((zor(a, _1, _1), zcmp(a, _1)), == 0);
	assert((zor(a, _1, _2), zcmp(a, _3)), == 0);
	assert((zor(a, _1, _3), zcmp(a, _3)), == 0);
	assert((zor(a, _2, _2), zcmp(a, _2)), == 0);
	assert((zor(a, _2, _3), zcmp(a, _3)), == 0);
	assert((zor(a, _3, _3), zcmp(a, _3)), == 0);

	assert((zxor(a, _0, _0), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zxor(a, _0, _1), zcmp(a, _1)), == 0);
	assert((zxor(a, _0, _2), zcmp(a, _2)), == 0);
	assert((zxor(a, _0, _3), zcmp(a, _3)), == 0);
	assert((zxor(a, _1, _1), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zxor(a, _1, _2), zcmp(a, _3)), == 0);
	assert((zxor(a, _1, _3), zcmp(a, _2)), == 0);
	assert((zxor(a, _2, _2), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zxor(a, _2, _3), zcmp(a, _1)), == 0);
	assert((zxor(a, _3, _3), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);

	zneg(b, _1);
	zneg(c, _3);
	zneg(_1, _1);
	zand(a, b, c);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, _1), == 0);
	zneg(_1, _1);
	assert((zand(a, b, _3), zcmp(a, _1)), == 0);
	assert((zand(a, _1, c), zcmp(a, _1)), == 0);
	assert((zand(a, _0, c), zcmp(a, _0)), == 0);
	assert((zand(a, b, _0), zcmp(a, _0)), == 0);

	zneg(b, _1);
	zneg(c, _2);
	zneg(_3, _3);
	zor(a, b, c);
	assert(zcmpmag(a, _3), == 0);
	assert(zcmp(a, _3), == 0);
	zor(a, b, _2);
	assert(zcmpmag(a, _3), == 0);
	assert(zcmp(a, _3), == 0);
	zor(a, _1, c);
	assert((zcmpmag(a, _3)), == 0);
	assert((zcmp(a, _3)), == 0);
	assert((zor(a, _0, c), zcmp(a, c)), == 0);
	assert((zor(a, b, _0), zcmp(a, b)), == 0);
	zneg(_3, _3);

	zneg(b, _1);
	zneg(c, _2);
	zxor(a, b, c);
	assert(zcmpmag(a, _3), == 0);
	assert(zcmp(a, _3), == 0);
	zneg(_3, _3);
	zxor(a, b, _2);
	assert(zcmpmag(a, _3), == 0);
	assert(zcmp(a, _3), == 0);
	zxor(a, _1, c);
	assert(zcmpmag(a, _3), == 0);
	assert(zcmp(a, _3), == 0);
	zxor(a, b, _0);
	assert(zcmpmag(a, b), == 0);
	assert(zcmp(a, b), == 0);
	zxor(a, _0, c);
	assert(zcmpmag(a, c), == 0);
	assert(zcmp(a, c), == 0);
	zneg(_3, _3);

	assert((zlsh(a, _0, 0), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zlsh(a, _0, 1), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zlsh(a, _1, 0), zcmp(a, _1)), == 0);
	assert((zlsh(a, _1, 1), zcmp(a, _2)), == 0);
	assert((zlsh(a, _1, 2), zcmp(a, _2)), > 0);
	assert((zlsh(a, _2, 0), zcmp(a, _2)), == 0);
	assert((zlsh(a, _2, 1), zcmp(a, _2)), > 0);

	zset(a, _0);
	assert((zlsh(a, a, 0), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zlsh(a, a, 1), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	zset(a, _1);
	assert((zlsh(a, a, 0), zcmp(a, _1)), == 0);
	assert((zlsh(a, a, 1), zcmp(a, _2)), == 0);
	assert((zlsh(a, a, 2), zcmp(a, _2)), > 0);
	zset(a, _2);
	assert((zlsh(a, a, 0), zcmp(a, _2)), == 0);
	assert((zlsh(a, a, 1), zcmp(a, _2)), > 0);

	assert((zrsh(a, _0, 0), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zrsh(a, _0, 1), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zrsh(a, _1, 0), zcmp(a, _1)), == 0);
	assert((zrsh(a, _1, 1), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zrsh(a, _1, 2), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zrsh(a, _2, 0), zcmp(a, _2)), == 0);
	assert((zrsh(a, _2, 1), zcmp(a, _1)), == 0);
	assert((zrsh(a, _2, 2), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);

	zset(a, _0);
	assert((zrsh(a, a, 0), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zrsh(a, a, 1), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	zset(a, _1);
	assert((zrsh(a, a, 0), zcmp(a, _1)), == 0);
	assert((zrsh(a, a, 1), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	assert((zrsh(a, a, 2), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);
	zset(a, _2);
	assert((zrsh(a, a, 0), zcmp(a, _2)), == 0);
	assert((zrsh(a, a, 1), zcmp(a, _1)), == 0);
	assert((zrsh(a, a, 2), zcmp(a, _0)), == 0);
	assert(zzero(a), == 1);

	assert(zbtest(_0, 0), == 0);
	assert(zbtest(_1, 0), == 1);
	assert(zbtest(_2, 0), == 0);
	assert(zbtest(_3, 0), == 1);
	assert(zbtest(_0, 1), == 0);
	assert(zbtest(_1, 1), == 0);
	assert(zbtest(_2, 1), == 1);
	assert(zbtest(_3, 1), == 1);
	assert(zbtest(_0, 2), == 0);
	assert(zbtest(_1, 2), == 0);
	assert(zbtest(_2, 2), == 0);
	assert(zbtest(_3, 2), == 0);

	znot(a, _2);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, _1), != 0);
	znot(a, a);
	assert(zcmp(a, _0), == 0);

	zsetu(a, 0xEEFF);
	zsetu(c, 0xEE);
	zsetu(d, 0xFF);
	zsplit(a, b, a, 8);
	assert(zcmpmag(a, c), == 0);
	assert(zcmpmag(b, d), == 0);
	zsetu(a, 0xEEFF);
	zsplit(b, a, a, 8);
	assert(zcmpmag(b, c), == 0);
	assert(zcmpmag(a, d), == 0);

	zmul(a, _2, _3);
	assert(zcmpi(a, 6), == 0);
	zneg(_3, _3);
	zmul(a, _2, _3);
	assert(zcmpi(a, -6), == 0);
	zneg(_3, _3);
	zneg(_2, _2);
	zmul(a, _2, _3);
	assert(zcmpi(a, -6), == 0);
	zneg(_3, _3);
	zmul(a, _2, _3);
	assert(zcmpi(a, 6), == 0);
	zneg(_3, _3);
	zneg(_2, _2);

	zmul(a, _3, _3);
	assert(zcmpi(a, 9), == 0);
	zsqr(a, _3);
	assert(zcmpi(a, 9), == 0);
	zneg(_3, _3);
	zmul(a, _3, _3);
	assert(zcmpi(a, 9), == 0);
	zsqr(a, _3);
	assert(zcmpi(a, 9), == 0);
	zneg(_3, _3);

	zseti(a, 8);
	zseti(b, 2);
	zdiv(c, a, b);
	assert(zcmpi(c, 4), == 0);
	zseti(b, -2);
	zdiv(c, a, b);
	assert(zcmpi(c, -4), == 0);
	zseti(a, -8);
	zseti(b, 2);
	zdiv(c, a, b);
	assert(zcmpi(c, -4), == 0);
	zseti(b, -2);
	zdiv(c, a, b);
	assert(zcmpi(c, 4), == 0);

	zseti(a, 1000);
	zseti(b, 10);
	zdiv(c, a, b);
	assert(zcmpi(c, 100), == 0);
	zseti(b, -10);
	zdiv(c, a, b);
	assert(zcmpi(c, -100), == 0);
	zseti(a, -1000);
	zseti(b, 10);
	zdiv(c, a, b);
	assert(zcmpi(c, -100), == 0);
	zseti(b, -10);
	zdiv(c, a, b);
	assert(zcmpi(c, 100), == 0);

	zseti(a, 7);
	zseti(b, 3);
	zmod(c, a, b);
	assert(zcmpi(c, 1), == 0);
	zseti(b, -3);
	zmod(c, a, b);
	assert(zcmpi(c, 1), == 0);
	zseti(a, -7);
	zseti(b, 3);
	zmod(c, a, b);
	assert(zcmpi(c, 1), == 0);
	zseti(b, -3);
	zmod(c, a, b);
	assert(zcmpi(c, 1), == 0);

	zseti(a, 7);
	zseti(b, 3);
	zdivmod(d, c, a, b);
	assert(zcmpi(d, 2), == 0);
	assert(zcmpi(c, 1), == 0);
	zseti(b, -3);
	zdivmod(d, c, a, b);
	assert(zcmpi(d, -2), == 0);
	assert(zcmpi(c, 1), == 0);
	zseti(a, -7);
	zseti(b, 3);
	zdivmod(d, c, a, b);
	assert(zcmpi(d, -2), == 0);
	assert(zcmpi(c, 1), == 0);
	zseti(b, -3);
	zdivmod(d, c, a, b);
	assert(zcmpi(d, 2), == 0);
	assert(zcmpi(c, 1), == 0);

	zseti(a, 10);
	zseti(b, -1);
	zpow(a, a, b);
	assert(zcmp(a, _0), == 0);

	zseti(a, 10);
	zseti(b, -1);
	zseti(a, 20);
	zmodpow(a, a, b, c);
	assert(zcmp(a, _0), == 0);

	zseti(a, 10);
	zseti(c, 100000L);
	zpowu(a, a, 5);
	assert(zcmpmag(a, c), == 0);
	assert(zcmp(a, c), == 0);

	zseti(a, -10);
	zseti(c, -100000L);
	zpowu(a, a, 5);
	assert(zcmpmag(a, c), == 0);
	assert(zcmp(a, c), == 0);

	zseti(a, -10);
	zseti(c, 10000L);
	zpowu(a, a, 4);
	assert(zcmpmag(a, c), == 0);
	assert(zcmp(a, c), == 0);

	zseti(a, 10);
	zseti(c, 3);
	zmodpowu(a, a, 5, c);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, _1), == 0);

	zseti(a, 10);
	zseti(b, 5);
	zseti(c, 100000L);
	zpow(a, a, b);
	assert(zcmpmag(a, c), == 0);
	assert(zcmp(a, c), == 0);

	zseti(a, -10);
	zseti(b, 5);
	zseti(c, -100000L);
	zpow(a, a, b);
	assert(zcmpmag(a, c), == 0);
	assert(zcmp(a, c), == 0);

	zseti(a, -10);
	zseti(b, 4);
	zseti(c, 10000L);
	zpow(a, a, b);
	assert(zcmpmag(a, c), == 0);
	assert(zcmp(a, c), == 0);

	zseti(a, 10);
	zseti(b, 5);
	zseti(c, 3);
	zmodpow(a, a, b, c);
	assert(zcmpmag(a, _1), == 0);
	assert(zcmp(a, _1), == 0);

	zseti(a, 102);
	zseti(b, 501);
	zseti(c, 5);
	zmodmul(a, a, b, c);
	assert(zcmp(a, _2), == 0);

	zseti(b, 2 * 3 * 3 * 7);
	zseti(c, 3 * 7 * 11);
	zseti(d, 3 * 7);
	assert((zgcd(a, _0, _0), zcmp(a, _0)), == 0);
	assert((zgcd(a, b, _0), zcmp(a, b)), == 0);
	assert((zgcd(a, _0, c), zcmp(a, c)), == 0);
	assert((zgcd(a, b, b), zcmp(a, b)), == 0);
	assert((zgcd(a, b, _2), zcmp(a, _2)), == 0);
	assert((zgcd(a, _2, b), zcmp(a, _2)), == 0);
	assert((zgcd(a, _2, _2), zcmp(a, _2)), == 0);
	assert((zgcd(a, c, _2), zcmp(a, _1)), == 0);
	assert((zgcd(a, _2, c), zcmp(a, _1)), == 0);
	assert((zgcd(a, b, _1), zcmp(a, _1)), == 0);
	assert((zgcd(a, _1, c), zcmp(a, _1)), == 0);
	assert((zgcd(a, _1, _1), zcmp(a, _1)), == 0);
	assert((zgcd(a, b, c), zcmp(a, d)), == 0);
	assert((zgcd(a, c, b), zcmp(a, d)), == 0);

	zsets(a, "1234");
	assert(zcmpi(a, 1234), == 0);
	zsets(b, "+1234");
	assert(zcmp(a, b), == 0);
	assert_zu(zstr_length(_0, 10), 1);
	assert_zu(zstr_length(_1, 10), 1);
	assert_zu(zstr_length(_2, 10), 1);
	assert_zu(zstr_length(_3, 10), 1);
	zneg(_2, _2);
	assert_zu(zstr_length(_2, 10), 2);
	zneg(_2, _2);
	assert_zu(zstr_length(a, 10), 4);
	zstr(a, buf);
	assert_s(buf, "1234");
	zsets(a, "-1234");
	zseti(b, -1234);
	zseti(c, 1234);
	assert(zcmp(a, _0), < 0);
	assert(zcmp(a, b), == 0);
	assert(zcmpmag(a, c), == 0);
	assert(zcmp(a, c), < 0);
	zstr(a, buf);
	assert_s(buf, "-1234");
	assert_s(zstr(a, buf), "-1234");

	zsetu(d, 100000UL);
	zrand(a, FAST_RANDOM, UNIFORM, d);
	assert(zcmp(a, _0), >= 0);
	assert(zcmp(a, d), <= 0);
	zrand(b, SECURE_RANDOM, UNIFORM, d);
	assert(zcmp(b, _0), >= 0);
	assert(zcmp(b, d), <= 0);
	zrand(c, FAST_RANDOM, UNIFORM, d);
	assert(zcmp(c, _0), >= 0);
	assert(zcmp(c, d), <= 0);
	assert(zcmp(a, b), != 0);
	assert(zcmp(a, c), != 0);
	assert(zcmp(b, c), != 0);

	assert((zseti(a, -5), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, -4), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, -3), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, -2), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, -1), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, 0), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, 1), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, 2), zptest(0, a, 100)), == PRIME);
	assert((zseti(a, 3), zptest(0, a, 100)), == PRIME);
	assert((zseti(a, 4), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, 5), zptest(0, a, 100)), != NONPRIME);
	assert((zseti(a, 6), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, 7), zptest(0, a, 100)), != NONPRIME);
	assert((zseti(a, 8), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, 9), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, 10), zptest(0, a, 100)), == NONPRIME);
	assert((zseti(a, 11), zptest(0, a, 100)), != NONPRIME);
	assert((zseti(a, 101), zptest(0, a, 100)), != NONPRIME);

done:
	zfree(a), zfree(b), zfree(c), zfree(d), zfree(_0), zfree(_1), zfree(_2), zfree(_3);
	zunsetup();
	return ret;
}
