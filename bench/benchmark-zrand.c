#include "util.h"


#define BENCHMARK(INSTRUCTION, FAST)\
	do {\
		i = FAST ? 1000000L : 1000L;\
		TIC;\
		while (i--) {\
			INSTRUCTION;\
		}\
		TOC;\
		printf("%s: %s %s\n",\
		       #INSTRUCTION, STIME, FAST ? "µs" : "ms");\
	} while (0)


int
main(int argc, char *argv[])
{
	z_t r, n;
	jmp_buf jmp;
	size_t i;

	benchmark_init();

	if (setjmp(jmp)) {
		zperror(argv[0]);
		return 1;
	}
	zsetup(jmp);
	zinit(r);
	zinit(n);

	zsetu(n, 1);
	zlsh(n, n, 64000L - 1L);
	zset(r, n);

	BENCHMARK(zrand(r, FAST_RANDOM, MODUNIFORM, n), 0);
	BENCHMARK(zrand(r, LIBC_RAND_RANDOM, MODUNIFORM, n), 0);
	BENCHMARK(zrand(r, LIBC_RANDOM_RANDOM, MODUNIFORM, n), 0);
	BENCHMARK(zrand(r, LIBC_RAND48_RANDOM, MODUNIFORM, n), 0);

	zfree(r);
	zfree(n);
	zunsetup();
	return 0;
	(void) argc;
}
