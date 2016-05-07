/* See LICENSE file for copyright and license details. */

ZAHL_INLINE void zinit(z_t a)         { a->alloced = 0; a->chars = 0; }
ZAHL_INLINE int zeven(z_t a)          { return !a->sign || (~a->chars[0] & 1); }
ZAHL_INLINE int zodd(z_t a)           { return a->sign && (a->chars[0] & 1); }
ZAHL_INLINE int zeven_nonzero(z_t a)  { return ~a->chars[0] & 1; }
ZAHL_INLINE int zodd_nonzero(z_t a)   { return a->chars[0] & 1; }
ZAHL_INLINE int zzero(z_t a)          { return !a->sign; }
ZAHL_INLINE int zsignum(z_t a)        { return a->sign; }
ZAHL_INLINE void zneg(z_t a, z_t b)   { ZAHL_SET(a, b); a->sign = -a->sign; }

#if 1 && (-1 & 1) /* In the future, tuning will select the fastest implementation. */
ZAHL_INLINE void zabs(z_t a, z_t b)   { ZAHL_SET(a, b); a->sign &= 1; }
#elif 1
ZAHL_INLINE void zabs(z_t a, z_t b)   { ZAHL_SET(a, b); if (ZAHL_LIKELY(a->sign < 0)) a->sign = 1; }
#else
ZAHL_INLINE void zabs(z_t a, z_t b)   { ZAHL_SET(a, b); a->sign = !!a->sign; }
#endif


#if ULONG_MAX != SIZE_MAX /* This variant should be equivalent to the second one if .sign was long. */
ZAHL_INLINE void
zswap(z_t a, z_t b)
{
	z_t t;
	ZAHL_SWAP(a, b, t, sign);
	ZAHL_SWAP(b, a, t, used);
	ZAHL_SWAP(a, b, t, alloced);
	ZAHL_SWAP(b, a, t, chars);
}
#else
ZAHL_INLINE void
zswap(z_t a_, z_t b_)
{
	register long t;
	long *a = (long *)a_;
	long *b = (long *)b_;
	t = a[0], a[0] = b[0], b[0] = t;
	t = b[1], b[1] = a[1], a[1] = t;
	t = a[2], a[2] = b[2], b[2] = t;
	t = b[3], b[3] = a[3], a[3] = t;
}
#endif


ZAHL_INLINE void
zset(z_t a, z_t b)
{
	if (ZAHL_UNLIKELY(b->sign == 0)) {
		a->sign = 0;
	} else {
		a->sign = b->sign;
		a->used = b->used;
		ZAHL_ENSURE_SIZE(a, b->used);
		libzahl_memcpy(a->chars, b->chars, b->used);
	}
}


ZAHL_INLINE void
zseti(z_t a, int64_t b)
{
	if (ZAHL_UNLIKELY(b >= 0)) {
		zsetu(a, (uint64_t)b);
		return;
	}
	ZAHL_ENSURE_SIZE(a, 1);
	ZAHL_SET_SIGNUM(a, -1);
	a->chars[0] = (zahl_char_t)-b;
	a->used = 1;
}


ZAHL_INLINE void
zsetu(z_t a, uint64_t b)
{
	if (ZAHL_UNLIKELY(!b)) {
		ZAHL_SET_SIGNUM(a, 0);
		return;
	}
	ZAHL_ENSURE_SIZE(a, 1);
	ZAHL_SET_SIGNUM(a, 1);
	a->chars[0] = (zahl_char_t)b;
	a->used = 1;
}


ZAHL_INLINE size_t
zlsb(z_t a)
{
	size_t i = 0;
	if (ZAHL_UNLIKELY(zzero(a)))
		return SIZE_MAX;
	for (; !a->chars[i]; i++);
	i *= 8 * sizeof(zahl_char_t);
	ZAHL_ADD_CTZ(i, a->chars[i]);
	return i;
}


ZAHL_INLINE size_t
zbits(z_t a)
{
	size_t rc;
	if (ZAHL_UNLIKELY(zzero(a)))
		return 1;
	while (!a->chars[a->used - 1])  a->used--; /* TODO should not be necessary */
	rc = a->used * 8 * sizeof(zahl_char_t);
	ZAHL_SUB_CLZ(rc, a->chars[a->used - 1]);
	return rc;
}


ZAHL_INLINE int
zcmpmag(z_t a, z_t b)
{
	size_t i, j;
	if (ZAHL_UNLIKELY(zzero(a)))  return -!zzero(b);
	if (ZAHL_UNLIKELY(zzero(b)))  return 1;
	i = a->used - 1;
	j = b->used - 1;
#if 0 /* TODO this should be sufficient. */
	if (i != j)
		return (i > j) * 2 - 1;
#else
	for (; i > j; i--) {
		if (a->chars[i])
			return +1;
		a->used--;
	}
	for (; j > i; j--) {
		if (b->chars[j])
			return -1;
		b->used--;
	}
#endif
	for (; i && a->chars[i] == b->chars[i]; i--);
	return a->chars[i] < b->chars[i] ? -1 : a->chars[i] > b->chars[i];
}


ZAHL_INLINE int
zcmp(z_t a, z_t b)
{
	if (zsignum(a) != zsignum(b))
		return zsignum(a) < zsignum(b) ? -1 : zsignum(a) > zsignum(b);
	return zsignum(a) * zcmpmag(a, b);
}


ZAHL_INLINE int
zcmpu(z_t a, uint64_t b)
{
	if (ZAHL_UNLIKELY(!b))
		return zsignum(a);
	if (ZAHL_UNLIKELY(zsignum(a) <= 0))
		return -1;
	while (!a->chars[a->used - 1])  a->used--; /* TODO should not be necessary */
	if (a->used > 1)
		return +1;
	return a->chars[0] < b ? -1 : a->chars[0] > b;
}


ZAHL_INLINE int
zcmpi(z_t a, int64_t b)
{
	if (ZAHL_UNLIKELY(!b))
		return zsignum(a);
	if (ZAHL_UNLIKELY(zzero(a)))
		return ZAHL_LIKELY(b < 0) ? 1 : -1;
	if (ZAHL_LIKELY(b < 0)) {
		if (zsignum(a) > 0)
			return +1;
		while (!a->chars[a->used - 1])  a->used--; /* TODO should not be necessary */
		if (a->used > 1)
			return -1;
		return a->chars[0] > (zahl_char_t)-b ? -1 : a->chars[0] < (zahl_char_t)-b;
	} else {
		if (zsignum(a) < 0)
			return -1;
		while (!a->chars[a->used - 1])  a->used--; /* TODO should not be necessary */
		if (a->used > 1)
			return +1;
		return a->chars[0] < (zahl_char_t)b ? -1 : a->chars[0] > (zahl_char_t)b;
	}
}


ZAHL_INLINE void
zbset(z_t a, z_t b, size_t bit, int action)
{
	if (ZAHL_UNLIKELY(a != b))
		zset(a, b);

#ifdef ZAHL_CONST_P
	if (ZAHL_CONST_P(action) && ZAHL_CONST_P(bit)) {
		zahl_char_t mask = 1;
		if (zzero(a) || ZAHL_FLOOR_BITS_TO_CHARS(bit) >= a->used) {
			if (!action)
				return;
			goto fallback;
		}
		mask <<= ZAHL_BITS_IN_LAST_CHAR(bit);
		if (action > 0) {
			a->chars[ZAHL_FLOOR_BITS_TO_CHARS(bit)] |= mask;
			return;
		} else if (action < 0) {
			a->chars[ZAHL_FLOOR_BITS_TO_CHARS(bit)] ^= mask;
		} else {
			a->chars[ZAHL_FLOOR_BITS_TO_CHARS(bit)] &= ~mask;
		}
		ZAHL_TRIM_AND_ZERO(a);
		return;
	}
fallback:
#endif

	if (action > 0)
		zbset_ll_set(a, bit);
	else if (action < 0)
		zbset_ll_flip(a, bit);
	else
		zbset_ll_clear(a, bit);
}


ZAHL_O3 ZAHL_INLINE int
zbtest(z_t a, size_t bit)
{
	size_t chars;
	if (ZAHL_UNLIKELY(zzero(a)))
		return 0;

	chars = ZAHL_FLOOR_BITS_TO_CHARS(bit);
	if (ZAHL_UNLIKELY(chars >= a->used))
		return 0;

	bit &= ZAHL_BITS_IN_LAST_CHAR(bit);
	return (a->chars[chars] >> bit) & 1;
}


ZAHL_O3 ZAHL_INLINE void
zsplit(z_t high, z_t low, z_t a, size_t delim)
{
	if (ZAHL_UNLIKELY(high == a)) {
		ztrunc(low, a, delim);
		zrsh(high, a, delim);
	} else {
		zrsh(high, a, delim);
		ztrunc(low, a, delim);
	}
}


ZAHL_INLINE size_t
zsave(z_t a, void *buffer)
{
	if (ZAHL_LIKELY(buffer)) {
		char *buf = buffer;
		*((long *)buf)   = a->sign, buf += sizeof(long); /* Use `long` for alignment. */
		*((size_t *)buf) = a->used, buf += sizeof(size_t);
		if (ZAHL_LIKELY(!zzero(a))) {
			a->chars[a->used + 2] = 0;
			a->chars[a->used + 1] = 0;
			a->chars[a->used + 0] = 0;
			libzahl_memcpy((zahl_char_t *)buf, a->chars, a->used);
		}
	}
	return sizeof(long) + sizeof(size_t) +
		(zzero(a) ? 0 :((a->used + 3) & (size_t)~3) * sizeof(zahl_char_t));
}


ZAHL_INLINE void
zmul(z_t a, z_t b, z_t c)
{
	int b_sign, c_sign;
	b_sign = b->sign, b->sign *= b_sign;
	c_sign = c->sign, c->sign *= c_sign;
	zmul_ll(a, b, c);
	c->sign = c_sign;
	b->sign = b_sign;
	ZAHL_SET_SIGNUM(a, zsignum(b) * zsignum(c));
}


ZAHL_INLINE void
zsqr(z_t a, z_t b)
{
	if (ZAHL_UNLIKELY(zzero(b))) {
		ZAHL_SET_SIGNUM(a, 0);
	} else {
		zsqr_ll(a, b);
		ZAHL_SET_SIGNUM(a, 1);
	}
}


ZAHL_INLINE void
zdiv(z_t a, z_t b, z_t c)
{
	zdivmod(a, libzahl_tmp_div, b, c);
}


ZAHL_INLINE void
zmod(z_t a, z_t b, z_t c)
{
	zdivmod(libzahl_tmp_mod, a, b, c);
}
