/* See LICENSE file for copyright and license details. */

/* Warning: libzahl is not thread-safe. */
/* Caution: Do not use libzahl for cryptographic applications, use a specialised library. */

#ifndef ZAHL_H
#define ZAHL_H 1


#include <limits.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>



/* TODO these should be documented*/
#define ZAHL_VERSION_MAJOR       1L
#define ZAHL_VERSION_MINOR       1L
#define ZAHL_VERSION_PATCHLEVEL  0L
#define ZAHL_VERSION             (ZAHL_VERSION_MAJOR * 1000000L +  \
				  ZAHL_VERSION_MINOR * 1000L    +  \
				  ZAHL_VERSION_PATCHLEVEL)
#define ZAHL_VERSION_STRING      "1.1"
#define ZAHL_INTERNALS_VERSION   1
#define ZAHL_ZRANDDEV_COUNT      7
#define ZAHL_ZRANDDIST_COUNT     3
#define ZAHL_ZERROR_COUNT        6



#include "zahl/internals.h"



typedef struct zahl z_t[1];



enum zprimality {
	NONPRIME = 0,                   /* The number is definitely composite. */
	PROBABLY_PRIME,                 /* The number is probably prime. */
	PRIME                           /* The number is definitely prime. */
};

enum zranddev {
	FAST_RANDOM = 0,                /* Random numbers are generated directly from /dev/urandom. */
	SECURE_RANDOM,                  /* Random numbers are generated directly from /dev/random. */
	DEFAULT_RANDOM,                 /* Select the default random number generator. */
	FASTEST_RANDOM,                 /* Select the fastest random number generator. */
	LIBC_RAND_RANDOM,               /* Use rand(3). */
	LIBC_RANDOM_RANDOM,             /* Use random(3). */
	LIBC_RAND48_RANDOM              /* Use lrand48(3). */
};

enum zranddist {
	QUASIUNIFORM = 0,               /* Almost uniformly random, per the usual recommendation. */
	UNIFORM,                        /* Actually uniformly random. */
	MODUNIFORM                      /* Almost uniformly random, using the naïve approach of modulation. */
};

enum zerror {
	ZERROR_ERRNO_SET = 0,           /* Please refer to errno. */
	ZERROR_0_POW_0,                 /* Indeterminate form: 0:th power of 0. (Translatable to EDOM.) */
	ZERROR_0_DIV_0,                 /* Indeterminate form: 0 divided by 0. (Translatable to EDOM.) */
	ZERROR_DIV_0,                   /* Undefined result: Division by 0. (Translatable to EDOM.) */
	ZERROR_NEGATIVE,                /* Argument must be non-negative. (Translatable to EDOM or EINVAL.) */
	ZERROR_INVALID_RADIX            /* Radix must be at least 2. (Translatable to EINVAL.) */
};



/* The parameters in the functions below are numbers a, b, c, ... */


/* Library initialisation and destruction. */

void zsetup(jmp_buf);                   /* Prepare libzahl for use. */
void zunsetup(void);                    /* Free resources used by libzahl */


/* Memory functions. */

ZAHL_INLINE void zinit(z_t);            /* Prepare a for use. */
ZAHL_INLINE void zswap(z_t, z_t);       /* (a, b) := (b, a) */
void zfree(z_t);                        /* Free resources in a. */
ZAHL_INLINE size_t zsave(z_t, void *);  /* Store a into b (if !!b), and return number of written bytes. */
size_t zload(z_t, const void *);        /* Restore a from b, and return number of read bytes. */


/* Assignment functions. */

ZAHL_INLINE void zset(z_t, z_t);        /* a := b */
ZAHL_INLINE void zsetu(z_t, uint64_t);  /* a := b */
ZAHL_INLINE void zseti(z_t, int64_t);   /* a := b */


/* Comparison functions. */

ZAHL_INLINE int zcmp(z_t, z_t);         /* signum (a - b) */
ZAHL_INLINE int zcmpu(z_t, uint64_t);   /* signum (a - b) */
ZAHL_INLINE int zcmpi(z_t, int64_t);    /* signum (a - b) */
ZAHL_INLINE int zcmpmag(z_t, z_t);      /* signum (|a| - |b|) */


/* Arithmetic functions. */

ZAHL_INLINE void zabs(z_t, z_t);        /* a := |b| */
ZAHL_INLINE void zneg(z_t, z_t);        /* a := -b */
void zadd(z_t, z_t, z_t);               /* a := b + c */
void zsub(z_t, z_t, z_t);               /* a := b - c */
ZAHL_INLINE void zmul(z_t, z_t, z_t);   /* a := b * c */
void zmodmul(z_t, z_t, z_t, z_t);       /* a := (b * c) % d */
ZAHL_INLINE void zdiv(z_t, z_t, z_t);   /* a := b / c */
void zdivmod(z_t, z_t, z_t, z_t);       /* a := c / d, b = c % d */
ZAHL_INLINE void zmod(z_t, z_t, z_t);   /* a := b % c */
ZAHL_INLINE void zsqr(z_t, z_t);        /* a := b² */
void zmodsqr(z_t, z_t, z_t);            /* a := b² % c */
void zpow(z_t, z_t, z_t);               /* a := b ↑ c */
void zmodpow(z_t, z_t, z_t, z_t);       /* a := (b ↑ c) % d */
void zpowu(z_t, z_t, unsigned long long int);
void zmodpowu(z_t, z_t, unsigned long long int, z_t);

/* These are used internally and may be removed in a future version. */
void zadd_unsigned(z_t, z_t, z_t);      /* a := |b| + |c| */
void zsub_unsigned(z_t, z_t, z_t);      /* a := |b| - |c| */
void zadd_unsigned_assign(z_t, z_t);    /* a := |a| + |b| */
void zsub_nonnegative_assign(z_t, z_t); /* a := a - b, assuming a ≥ b ≥ 0 */
void zsub_positive_assign(z_t, z_t);    /* a := a - b, assuming a > b > 0 */


/* Bitwise operations. */

void zand(z_t, z_t, z_t);               /* a := b & c */
void zor(z_t, z_t, z_t);                /* a := b | c */
void zxor(z_t, z_t, z_t);               /* a := b ^ c */
void znot(z_t, z_t);                    /* a := ~b */
void zlsh(z_t, z_t, size_t);            /* a := b << c */
void zrsh(z_t, z_t, size_t);            /* a := b >> c */
void ztrunc(z_t, z_t, size_t);          /* a := b & ((1 << c) - 1) */
ZAHL_INLINE void zsplit(z_t, z_t, z_t, size_t);
                                        /* a := c >> d, b := c - (a << d) */
ZAHL_INLINE int zbtest(z_t, size_t);    /* (a >> b) & 1 */
ZAHL_INLINE size_t zlsb(z_t);           /* Index of first set bit, SIZE_MAX if none are set. */
ZAHL_INLINE size_t zbits(z_t);          /* ⌊log₂ |a|⌋ + 1, 1 if a = 0 */

/* If d > 0: a := b | (1 << c), if d = 0: a := b & ~(1 << c), if d < 0: a := b ^ (1 << c) */
ZAHL_INLINE void zbset(z_t, z_t, size_t, int);


/* Number theory. */

ZAHL_INLINE int zeven(z_t);             /* Is a even? */
ZAHL_INLINE int zodd(z_t);              /* Is a odd? */
ZAHL_INLINE int zeven_nonzero(z_t);     /* Is a even? Assumes a ≠ 0. */
ZAHL_INLINE int zodd_nonzero(z_t);      /* Is a odd? Assumes a ≠ 0. */
ZAHL_INLINE int zzero(z_t);             /* Is a zero? */
ZAHL_INLINE int zsignum(z_t);           /* a/|a|, 0 if a is zero. */
void zgcd(z_t, z_t, z_t);               /* a := gcd(b, c) */

/* NONPRIME if b ∉ ℙ, PROBABLY_PRIME, if b ∈ ℙ with (1 − 4↑−c) certainty, 2 if PRIME ∈ ℙ.
 * If NONPRIME is returned the witness of b's compositeness is stored in a. */
enum zprimality zptest(z_t, z_t, int);


/* Random number generation. */

/* Pick a randomly from [0, d] ∩ ℤ. */
void zrand(z_t, enum zranddev, enum zranddist, z_t);


/* String conversion. */

char *zstr(z_t, char *, size_t);        /* Write a in decimal onto b. c is the output size or 0. */
int zsets(z_t, const char *);           /* a := b */

/* Length of a in radix b. */
size_t zstr_length(z_t, unsigned long long int);


/* Error handling functions. */

enum zerror zerror(const char **);      /* Return the current error code, and unless !a, a description in *a. */
void zperror(const char *);             /* Identical to perror(3p) except it supports libzahl errors. */



/* Low-level functions. [Do not count on these to be retained between different versions of libzahl.] */

void zbset_ll_set(z_t, size_t);         /* zbset(a, a, b, 1) */
void zbset_ll_clear(z_t, size_t);       /* zbset(a, a, b, 0) */
void zbset_ll_flip(z_t, size_t);        /* zbset(a, a, b, -1) */
void zmul_ll(z_t, z_t, z_t);            /* zmul for non-negative operands */
void zsqr_ll(z_t, z_t);                 /* zsqr for non-negative operand */



#include "zahl/inlines.h"


#endif
