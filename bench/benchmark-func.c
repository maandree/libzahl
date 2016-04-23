#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef BENCHMARK_LIB
# include BENCHMARK_LIB
#else
# include "../zahl.h"
# define BIGINT_LIBRARY "libzahl"
#endif

enum {
	HIGH_ONLY,
	HIGH_AND_LOW,
	HALF,
	FULL
};

struct function {
	const char *name;
	void (*f)(z_t *, z_t *, struct function *);
	size_t a_start;
	size_t a_end;
	size_t a_step;
	int a_mode;
	size_t b_start;
	size_t b_end;
	size_t b_step;
	int b_mode;
	size_t runs;
	size_t measurements;
};

#if defined(__x86_64__)
# undef clock_t
# define clock_t unsigned long long int
static inline clock_t rdtsc(void)
{
  unsigned int low, high;
  __asm__ __volatile__ ("rdtsc" : "=a"(low), "=d"(high));
  return (clock_t)low | (((clock_t)high) << 32);
}
#else
# define rdtsc clock
#endif

#define M_MAX 200

static char buf[1000];
static z_t temp, temp2;
static clock_t start, end;
static unsigned long long int measurements[M_MAX];

#if 1
static int
measurementpcmp(const void *ap_, const void *bp_)
{
	const unsigned long long int *ap = ap_, *bp = bp_;
	return *ap < *bp ? -1 : *ap > *bp;
}
# if 0
static unsigned long long int
gettime(size_t m)
{
	qsort(measurements, m, sizeof(*measurements), measurementpcmp);
	if (m & 1)
		return measurements[m / 2];
	return (measurements[m / 2] + measurements[m / 2 - 1]) / 2;
}
# else
static unsigned long long int
gettime(size_t m)
{
#  define X 2 / 7
	size_t i = m * X, n = m - m * X;
	unsigned long long int tot = 0;
	qsort(measurements, m, sizeof(*measurements), measurementpcmp);
	for (; i < n; i++)
		tot += measurements[i];
	return tot / (n - m * X);
#  undef X
}
# endif
#elif 0
static unsigned long long int
gettime(size_t m)
{
	unsigned long long int tot = 0;
	size_t i = m;
	while (i--)
		tot += measurements[i];
	return tot / m;
}
#else
static unsigned long long int
gettime(size_t m)
{
	unsigned long long int best = ULLONG_MAX;
	size_t i = m;
	while (i--)
		if (best > measurements[i])
			best = measurements[i];
	return best;
}
#endif

#define FUNCTION_2D(NAME, INSTRUCTION, PREINSTRUCTION)\
	void\
	NAME(z_t *as, z_t* bs, struct function *f)\
	{\
		size_t i, j, k, n = f->a_end - f->a_start + 1;\
		z_t *a;\
		zmul(temp, as[n - 1], as[n - 1]);\
		zadd(temp, temp, temp);\
		for (i = 0; i < n; i += f->a_step) {\
			a = as + i;\
			zset(temp2, *a);\
			PREINSTRUCTION;\
			k = f->measurements;\
			while (k--) {\
				INSTRUCTION;\
				INSTRUCTION;\
				j = f->runs;\
				start = rdtsc();\
				while (j--) {\
					INSTRUCTION;\
				}\
				end = rdtsc();\
				measurements[k] = (unsigned long long int)(end - start);\
			}\
			printf("%llu\n", gettime(f->measurements));\
			a++;\
		}\
		(void) bs;\
	}

FUNCTION_2D(bench_zset,             zset(temp, *a),)
FUNCTION_2D(bench_zneg,             zneg(temp, *a),)
FUNCTION_2D(bench_zabs,             zabs(temp, *a),)
FUNCTION_2D(bench_self_zneg,        zneg(*a, *a),)
FUNCTION_2D(bench_self_zabs,        zabs(*a, *a),)
FUNCTION_2D(bench_zadd_unsigned,    zadd_unsigned(temp, *a, temp2),)
FUNCTION_2D(bench_zsub_unsigned,    zsub_unsigned(temp, *a, temp2),)
FUNCTION_2D(bench_zadd,             zadd(temp, *a, temp2),)
FUNCTION_2D(bench_zsub,             zsub(temp, *a, temp2),)
FUNCTION_2D(bench_zand,             zand(temp, *a, temp2),)
FUNCTION_2D(bench_zor,              zor(temp, *a, temp2),)
FUNCTION_2D(bench_zxor,             zxor(temp, *a, temp2),)
FUNCTION_2D(bench_znot,             znot(temp, *a),)
FUNCTION_2D(bench_zeven,            zeven(*a),)
FUNCTION_2D(bench_zodd,             zodd(*a),)
FUNCTION_2D(bench_zeven_nonzero,    zeven_nonzero(*a),)
FUNCTION_2D(bench_zodd_nonzero,     zodd_nonzero(*a),)
FUNCTION_2D(bench_zzero,            zzero(*a),)
FUNCTION_2D(bench_zsignum,          zsignum(*a),)
FUNCTION_2D(bench_zbits,            zbits(*a),)
FUNCTION_2D(bench_zlsb,             zlsb(*a),)
FUNCTION_2D(bench_zswap,            zswap(temp, *a),)
FUNCTION_2D(bench_zcmpmag,          zcmpmag(temp2, *a),)
FUNCTION_2D(bench_zcmp,             zcmp(temp2, *a),)
FUNCTION_2D(bench_sqr_zmul,         zmul(temp, *a, temp2),)
FUNCTION_2D(bench_zsqr,             zsqr(temp, *a),)
FUNCTION_2D(bench_zstr_length,      zstr_length(*a, 10),)
FUNCTION_2D(bench_zstr,             zstr(*a, buf),)
FUNCTION_2D(bench_zsave,            zsave(*a, buf),)
FUNCTION_2D(bench_zload,            zload(temp, buf), zsave(*a, buf))
FUNCTION_2D(bench_zbset_set,        zbset(temp, *a, 2, 1),)
FUNCTION_2D(bench_zbset_clear,      zbset(temp, *a, 2, 0),)
FUNCTION_2D(bench_zbset_flip,       zbset(temp, *a, 2, -1),)
FUNCTION_2D(bench_self_zbset_set,   zbset(temp2, temp2, 2, 1),)
FUNCTION_2D(bench_self_zbset_clear, zbset(temp2, temp2, 2, 0),)
FUNCTION_2D(bench_self_zbset_flip,  zbset(temp2, temp2, 2, -1),)
FUNCTION_2D(bench_zbtest,           zbtest(*a, 2),)
FUNCTION_2D(bench_zptest,           zptest(temp, *a, 5),)
FUNCTION_2D(bench_zsets,            zsets(temp, buf), zstr(*a, buf))
FUNCTION_2D(bench_zlsh,             zlsh(temp, *a, 1),)
FUNCTION_2D(bench_zrsh,             zrsh(temp, *a, 1),)

/* TODO
	ztrunc
	self_ztrunc
	zsplit
	zgcd
	zpow
	zpowu
	zmodpow
	zmodpowu
	zrand
	zdiv
	zmod
	zdivmod
	zmul
	zmodmul
	sqr_zmodmul
	zmodsqr
	zdiv
	zmod
	zdivmod
*/

#define F(f) #f, bench_##f
struct function functions[] = {
	{F(zset),          1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zneg),          1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zabs),          1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(self_zneg),     1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(self_zabs),     1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zadd_unsigned), 1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zsub_unsigned), 1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zadd),          1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zsub),          1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zand),          1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zor),           1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zxor),          1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(znot),          1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zeven),         1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zodd),          1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zeven_nonzero), 1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zodd_nonzero),  1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zzero),         1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zsignum),       1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zbits),         1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zlsb),          1, 4097, 64, HIGH_ONLY, 0, 0, 0, 0, 1000, M_MAX},
	{F(zswap),         1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zcmpmag),       1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zcmp),          1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(sqr_zmul),      1, 4097, 64, FULL,      0, 0, 0, 0, 10,   20},
	{F(zsqr),          1, 4097, 64, FULL,      0, 0, 0, 0, 10,   20},
	{F(zstr_length),   1, 4097, 64, FULL,      0, 0, 0, 0, 10,   20},
	{F(zstr),          1, 4097, 64, FULL,      0, 0, 0, 0, 10,   20},
	{F(zsave),         1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},
	{F(zload),         1, 4097, 64, FULL,      0, 0, 0, 0, 1000, M_MAX},

	{0, 0,             0, 0,    0,  0,         0, 0, 0, 0, 0,    0}
};
#undef F

static z_t *
create_ints(size_t start, size_t end, int mode)
{
	z_t *array = malloc((end - start + 1) * sizeof(z_t));
	z_t *rc = array;
	ssize_t n;
	for (; start <= end; start++, array++) {
		zinit(*array);
		switch (mode) {
		case HIGH_ONLY:
			zsetu(temp, 1);
			zlsh(*array, temp, start - 1);
			break;
		case HIGH_AND_LOW:
			zsetu(temp, 1);
			zlsh(*array, temp, start - 1);
			if (start > 1)
				zadd(*array, *array, temp);
			break;
		case HALF:
			n = (ssize_t)start;
			zsetu(temp, 1 << (~start & 1));
			zsetu(*array, 0);
			for (; n > 0; n -= 2) {
				zlsh(*array, *array, 2);
				zadd(*array, *array, temp);
			}
			break;
		case FULL:
			zsetu(temp, 1);
			zlsh(*array, temp, start);
			zsub(*array, *array, temp);
			break;
		default:
			abort();
		}
	}
	return rc;
}

static void
destroy_ints(z_t *array, size_t start, size_t end)
{
	z_t *array_ = array;
	for (; start <= end; start++)
		zfree(*array++);
	free(array_);
}

int
main(int argc, char *argv[])
{
	struct function *fs = functions;
	jmp_buf jmp;
	z_t *as = 0, *bs = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s function\n", *argv);
		return 2;
	}

	if (setjmp(jmp)) {
		zperror(argv[0]);
		return 1;
	}
	zsetup(jmp);
	printf("%s\n", BIGINT_LIBRARY);
	zinit(temp);
	zinit(temp2);

	for (; fs->name && strcmp(fs->name, argv[1]); fs++);
	if (!fs->name) {
		fprintf(stderr, "%s: function not recognised: %s\n", *argv, argv[1]);
		return 2;
	}

	if (fs->b_end) {
		as = create_ints(fs->a_start, fs->a_end, fs->a_mode);
		bs = create_ints(fs->b_start, fs->b_end, fs->b_mode);
		printf("3\n%zu %zu %zu\n%zu %zu %zu\n",
		       fs->a_start, fs->a_end, fs->a_step,
		       fs->b_start, fs->b_end, fs->b_step);
	} else {
		as = create_ints(fs->a_start, fs->a_end, fs->a_mode);
		printf("2\n%zu %zu %zu\n", fs->a_start, fs->a_end, fs->a_step);
	}
	fs->f(as, bs, fs);

	if (as)
		destroy_ints(as, fs->a_start, fs->a_end);
	if (bs)
		destroy_ints(bs, fs->b_start, fs->b_end);

	zfree(temp);
	zfree(temp2);
	zunsetup();
	return 0;
}
