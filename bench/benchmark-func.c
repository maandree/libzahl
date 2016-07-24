#include "util.h"

#include <limits.h>


#if !defined(USE_MEDIAN) && !defined(USE_MID_7TH_AVERAGE) && !defined(USE_AVERAGE) && !defined(USE_LOWEST)
# define USE_MID_7TH_AVERAGE
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
	size_t b_start;
	size_t b_end;
	size_t b_step;
	int a_mode;
	int b_mode;
	size_t runs;
	size_t measurements;
};

#define M_MAX 200


static char buf[2000];
static z_t temp, temp2;
static unsigned long long int measurements[M_MAX];


#if defined(USE_MEDIAN) || defined(USE_MID_7TH_AVERAGE)
static int
measurementpcmp(const void *ap_, const void *bp_)
{
	const unsigned long long int *ap = ap_, *bp = bp_;
	return *ap < *bp ? -1 : *ap > *bp;
}
# if defined(USE_MEDIAN)
static unsigned long long int
gettime(size_t m)
{
	qsort(measurements, m, sizeof(*measurements), measurementpcmp);
	if (m & 1)
		return measurements[m / 2];
	return (measurements[m / 2] + measurements[m / 2 - 1]) / 2;
}
# else /* if defined(USE_MID_7TH_AVERAGE) */
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
#elif defined(USE_AVERAGE)
static unsigned long long int
gettime(size_t m)
{
	unsigned long long int tot = 0;
	size_t i = m;
	while (i--)
		tot += measurements[i];
	return tot / m;
}
#else /* if defined(USE_LOWEST) */
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


#define FUNCTION_1D(NAME, INSTRUCTION, PREINSTRUCTION)\
	static void\
	NAME(z_t *as, z_t* bs, struct function *f)\
	{\
		size_t i, j;\
		PREINSTRUCTION;\
		i = f->measurements;\
		while (i--) {\
			(void)INSTRUCTION;\
			(void)INSTRUCTION;\
			j = f->runs;\
			TIC;\
			while (j--) {\
				(void)INSTRUCTION;\
			}\
			TOC;\
			measurements[i] = TICKS;\
		}\
		printf("%llu\n", gettime(f->measurements));\
		(void) as;\
		(void) bs;\
	}

#define FUNCTION_2D(NAME, INSTRUCTION, PREINSTRUCTION)\
	static void\
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
				(void)INSTRUCTION;\
				(void)INSTRUCTION;\
				j = f->runs;\
				TIC;\
				while (j--) {\
					(void)INSTRUCTION;\
				}\
				TOC;\
				measurements[k] = TICKS;\
			}\
			printf("%llu\n", gettime(f->measurements));\
			a++;\
		}\
		(void) bs;\
	}

#if defined(FINE_GRAINED)
# define FAST1D()   0, 0,    0,  0, 0, 0, 0, 0, 1000, M_MAX
# define FAST2D(P)  1, 4096, 1,  0, 0, 0, P, 0, 1000, M_MAX
# define SLOW2D(P)  1, 4096, 1,  0, 0, 0, P, 0, 10,   20
#else
# define FAST1D()   0, 0,    0,  0, 0, 0, 0, 0, 1000, M_MAX
# define FAST2D(P)  1, 4097, 64, 0, 0, 0, P, 0, 1000, M_MAX
# define SLOW2D(P)  1, 4097, 64, 0, 0, 0, P, 0, 10,   20
#endif

#define LIST_1D_FUNCTIONS\
	X(pos_zseti,        FAST1D(),          zseti(temp, 1000000000LL),)\
	X(zseti,            FAST1D(),          zseti(temp, -1000000000LL),)\
	X(zsetu,            FAST1D(),          zsetu(temp, 1000000000ULL),)

#define LIST_2D_FUNCTIONS\
	X(zset,             FAST2D(FULL),      zset(temp, *a),)\
	X(zneg,             FAST2D(FULL),      zneg(temp, *a),)\
	X(zabs,             FAST2D(FULL),      zabs(temp, *a),)\
	X(self_zneg,        FAST2D(FULL),      zneg(*a, *a),)\
	X(self_zabs,        FAST2D(FULL),      zabs(*a, *a),)\
	X(zadd_unsigned,    FAST2D(FULL),      zadd_unsigned(temp, *a, temp2),)\
	X(zsub_unsigned,    FAST2D(FULL),      zsub_unsigned(temp, *a, temp2),)\
	X(zadd,             FAST2D(FULL),      zadd(temp, *a, temp2),)\
	X(zsub,             FAST2D(FULL),      zsub(temp, *a, temp2),)\
	X(zand,             FAST2D(FULL),      zand(temp, *a, temp2),)\
	X(zor,              FAST2D(FULL),      zor(temp, *a, temp2),)\
	X(zxor,             FAST2D(FULL),      zxor(temp, *a, temp2),)\
	X(znot,             FAST2D(FULL),      znot(temp, *a),)\
	X(zeven,            FAST2D(FULL),      zeven(*a),)\
	X(zodd,             FAST2D(FULL),      zodd(*a),)\
	X(zeven_nonzero,    FAST2D(FULL),      zeven_nonzero(*a),)\
	X(zodd_nonzero,     FAST2D(FULL),      zodd_nonzero(*a),)\
	X(zzero,            FAST2D(FULL),      zzero(*a),)\
	X(zsignum,          FAST2D(FULL),      zsignum(*a),)\
	X(zbits,            FAST2D(FULL),      zbits(*a),)\
	X(zlsb,             FAST2D(HIGH_ONLY), zlsb(*a),)\
	X(zswap,            FAST2D(FULL),      zswap(temp, *a),)\
	X(zcmpmag,          FAST2D(FULL),      zcmpmag(temp2, *a),)\
	X(zcmp,             FAST2D(FULL),      zcmp(temp2, *a),)\
	X(pos_zcmpi,        FAST2D(FULL),      zcmpi(*a, 1000000000LL),)\
	X(zcmpi,            FAST2D(FULL),      zcmpi(*a, -1000000000LL),)\
	X(zcmpu,            FAST2D(FULL),      zcmpu(*a, 1000000000ULL),)\
	X(sqr_zmul,         SLOW2D(FULL),      zmul(temp, *a, temp2),)\
	X(zsqr,             SLOW2D(FULL),      zsqr(temp, *a),)\
	X(zstr_length,      SLOW2D(FULL),      zstr_length(*a, 10),)\
	X(zstr,             SLOW2D(FULL),      zstr(*a, buf, sizeof(buf) - 1),)\
	X(auto_zstr,        SLOW2D(FULL),      zstr(*a, buf, 0),)\
	X(zsave,            FAST2D(FULL),      zsave(*a, buf),)\
	X(zload,            FAST2D(FULL),      zload(temp, buf), zsave(*a, buf))\
	X(zbset_set,        FAST2D(FULL),      zbset(temp, *a, 2, 1),)\
	X(zbset_clear,      FAST2D(FULL),      zbset(temp, *a, 2, 0),)\
	X(zbset_flip,       FAST2D(FULL),      zbset(temp, *a, 2, -1),)\
	X(self_zbset_set,   FAST2D(FULL),      zbset(temp2, temp2, 2, 1),)\
	X(self_zbset_clear, FAST2D(FULL),      zbset(temp2, temp2, 2, 0),)\
	X(self_zbset_flip,  FAST2D(FULL),      zbset(temp2, temp2, 2, -1),)\
	X(zbtest,           FAST2D(FULL),      zbtest(*a, 2),)\
	X(zptest,           FAST2D(FULL),      zptest(temp, *a, 5),)\
	X(zsets,            FAST2D(FULL),      zsets(temp, buf), zstr(*a, buf, sizeof(buf) - 1))\
	X(zlsh,             FAST2D(FULL),      zlsh(temp, *a, 1),)\
	X(zrsh,             FAST2D(FULL),      zrsh(temp, *a, 1),)\
	X(ztrunc,           FAST2D(FULL),      ztrunc(temp, *a, i / 2),)\
	X(self_ztrunc,      FAST2D(FULL),      ztrunc(*a, *a, i),)\
	X(zsplit,           FAST2D(FULL),      zsplit(temp, temp2, *a, i / 2),)

/* TODO
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

#define X(FN, A, F1, F2)  FUNCTION_1D(bench_##FN, F1, F2)
LIST_1D_FUNCTIONS
#undef X
#define X(FN, A, F1, F2)  FUNCTION_2D(bench_##FN, F1, F2)
LIST_2D_FUNCTIONS
#undef X

struct function functions[] = {
#define X(FN, A, F1, F2)  {#FN, bench_##FN, A},
LIST_1D_FUNCTIONS
LIST_2D_FUNCTIONS
#undef X
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


static z_t *
create_ints(size_t start, size_t end, int mode)
{
	z_t *array = malloc((++end - start) * sizeof(z_t));
	z_t *rc = array;
	ssize_t n;
	for (; start < end; start++, array++) {
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
	static struct function *fs = functions;
	static z_t *as = 0, *bs = 0;
	jmp_buf jmp;

	if (argc != 2) {
		fprintf(stderr, "usage: %s function\n", *argv);
		return 2;
	}

	benchmark_init();

	if (setjmp(jmp)) {
		zperror(argv[0]);
		return 1;
	}
	zsetup(jmp);
	printf("%s%s\n", BIGINT_LIBRARY, LIBRARY_SUFFIX);
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
	} else if (fs->a_end) {
		as = create_ints(fs->a_start, fs->a_end, fs->a_mode);
		printf("2\n%zu %zu %zu\n", fs->a_start, fs->a_end, fs->a_step);
	} else {
		printf("1\n");
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
