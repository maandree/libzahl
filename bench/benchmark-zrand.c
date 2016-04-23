#include <time.h>
#include <stdio.h>

#include "../zahl.h"


#ifndef CLOCK_MONOTONIC_RAW
# define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif


#define BENCHMARK(INSTRUCTION, FAST)\
	do {\
		i = FAST ? 1000000L : 1000L;\
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);\
		while (i--) {\
			INSTRUCTION;\
		}\
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);\
		end.tv_sec -= start.tv_sec;\
		end.tv_nsec -= start.tv_nsec;\
		if (end.tv_nsec < 0) {\
			end.tv_nsec += 1000000000L;\
			end.tv_sec -= 1;\
		}\
		printf("%s: %lli.%09li %s\n",\
		       #INSTRUCTION,\
		       (long long int)(end.tv_sec), end.tv_nsec,\
		       FAST ? "µs" : "ms");\
	} while (0)


int
main(int argc, char *argv[])
{
	struct timespec start, end;
	z_t r, n;
	jmp_buf jmp;
	size_t i;

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
