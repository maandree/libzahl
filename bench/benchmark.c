#include "util.h"


#define BENCHMARK(INSTRUCTION, FAST)\
	do {\
		i = FAST ? 1000000L : 1000L;\
		TIC;\
		while (i--) {\
			(void)INSTRUCTION;\
		}\
		TOC;\
		printf("%s: %s %s (152 bits)\n",\
		       #INSTRUCTION, STIME, FAST ? "µs" : "ms");\
	} while (0)


int
main(int argc, char *argv[])
{
	char buf[2000];
	z_t a, b, c, d, tiny;
	jmp_buf jmp;
	size_t i;

	benchmark_init();

	if (setjmp(jmp)) {
		zperror(argv[0]);
		return 1;
	}
	zsetup(jmp);
	zinit(a);
	zinit(b);
	zinit(c);
	zinit(d);
	zinit(tiny);

	zsets(a, "5495468234592964023447280368442884381000481887");
	zsets(b, "4781084818570683458641843084358135840548636081");
	zsets(tiny, "5");

	BENCHMARK(zset(c, a), 1);
	BENCHMARK(zseti(c, 1000000000LL), 1);
	BENCHMARK(zsetu(c, 1000000000ULL), 1);
	BENCHMARK(zneg(c, a), 1);
	BENCHMARK(zneg(a, a), 1);
	BENCHMARK(zabs(c, a), 1);
	BENCHMARK(zabs(a, a), 1);
	BENCHMARK(zadd_unsigned(c, a, b), 1);
	BENCHMARK(zsub_unsigned(c, a, b), 1);
	BENCHMARK(zadd(c, a, b), 1);
	BENCHMARK(zsub(c, a, b), 1);
	BENCHMARK(zand(c, a, b), 1);
	BENCHMARK(zor(c, a, b), 1);
	BENCHMARK(zxor(c, a, b), 1);
	BENCHMARK(znot(c, a), 1);
	BENCHMARK(zeven(a), 1);
	BENCHMARK(zodd(a), 1);
	BENCHMARK(zeven_nonzero(a), 1);
	BENCHMARK(zodd_nonzero(a), 1);
	BENCHMARK(zzero(a), 1);
	BENCHMARK(zsignum(a), 1);
	BENCHMARK(zbits(a), 1);
	BENCHMARK(zlsb(a), 1);
	BENCHMARK(zswap(a, b), 1);
	BENCHMARK(zlsh(c, a, 76), 1);
	BENCHMARK(zrsh(c, a, 76), 1);
	BENCHMARK(ztrunc(c, a, 76), 1);
	BENCHMARK(ztrunc(c, c, 76), 1);
	BENCHMARK(zsplit(c, d, a, 76), 1);
	BENCHMARK(zcmpmag(a, b), 1);
	BENCHMARK(zcmp(a, b), 1);
	BENCHMARK(zcmpi(a, 1000000000LL), 1);
	BENCHMARK(zcmpi(a, -1000000000LL), 1);
	BENCHMARK(zcmpu(a, 1000000000ULL), 1);
	BENCHMARK(zbset(c, a, 76, 1), 1);
	BENCHMARK(zbset(a, a, 76, 1), 1);
	BENCHMARK(zbset(c, a, 76, 0), 1);
	BENCHMARK(zbset(c, c, 76, 0), 1);
	BENCHMARK(zbset(c, a, 76, -1), 1);
	BENCHMARK(zbset(a, a, 76, -1), 1);
	BENCHMARK(zbtest(a, 76), 1);
#ifndef HEBIMATH /* These take too long in hebimath because of inefficient division. */
	BENCHMARK(zgcd(c, a, b), 0);
#endif
	BENCHMARK(zmul(c, a, b), 0);
	BENCHMARK(zmul(c, a, a), 0);
	BENCHMARK(zsqr(c, a), 0);
#ifndef HEBIMATH /* Ditto. */
	zsets(d, "1484298084218938358480511181388394862858002249");
	BENCHMARK(zmodmul(c, a, b, d), 0);
	BENCHMARK(zmodmul(c, a, a, d), 0);
	BENCHMARK(zmodsqr(c, a, d), 0);
	BENCHMARK(zmodmul(c, a, b, tiny), 0);
	BENCHMARK(zmodmul(c, a, a, tiny), 0);
	BENCHMARK(zmodsqr(c, a, tiny), 0);
	zsets(d, "12");
	BENCHMARK(zpow(c, a, d), 0);   /* Memory corruption when using hebimath. */
	BENCHMARK(zpowu(c, a, 12), 0); /* Memory corruption when using hebimath. */
	BENCHMARK(zmodpow(c, a, d, b), 0);
	BENCHMARK(zmodpowu(c, a, 12, b), 0);
#endif
	BENCHMARK(zsets(c, "5495468234592964023447280368442884381000481887"), 0);
	BENCHMARK(zstr_length(a, 10), 0);
	BENCHMARK(zstr(a, buf, 0), 0);
	BENCHMARK(zstr(a, buf, sizeof(buf) - 1), 0);
	BENCHMARK(zrand(c, DEFAULT_RANDOM, QUASIUNIFORM, a), 0);
	BENCHMARK(zrand(c, DEFAULT_RANDOM, UNIFORM, a), 0);
	BENCHMARK(zrand(c, DEFAULT_RANDOM, MODUNIFORM, a), 0);
	BENCHMARK(zptest(d, a, 5), 0);
	BENCHMARK(zsave(a, buf), 1);
	BENCHMARK(zload(a, buf), 1);
	BENCHMARK(zdiv(c, a, b), 1);
	BENCHMARK(zmod(c, a, b), 1);
	BENCHMARK(zdivmod(c, d, a, b), 1);
#ifndef HEBIMATH /* Ditto. */
	BENCHMARK(zdiv(c, a, tiny), 0);
	BENCHMARK(zmod(c, a, tiny), 0);
	BENCHMARK(zdivmod(c, d, a, tiny), 0);
#endif

	zfree(a);
	zfree(b);
	zfree(c);
	zfree(d);
	zfree(tiny);
	zunsetup();
	return 0;
	(void) argc;
}
