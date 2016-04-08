/* See LICENSE file for copyright and license details. */

ZAHL_INLINE void zinit(z_t a)         { a->alloced = 0; a->chars = 0; }
ZAHL_INLINE int zeven(z_t a)          { return !a->sign || !(a->chars[0] & 1); }
ZAHL_INLINE int zodd(z_t a)           { return a->sign && (a->chars[0] & 1); }
ZAHL_INLINE int zeven_nonzero(z_t a)  { return !(a->chars[0] & 1); }
ZAHL_INLINE int zodd_nonzero(z_t a)   { return (a->chars[0] & 1); }
ZAHL_INLINE int zzero(z_t a)          { return !a->sign; }
ZAHL_INLINE int zsignum(z_t a)        { return a->sign; }
ZAHL_INLINE void zabs(z_t a, z_t b)   { ZAHL_SET(a, b); a->sign = !!a->sign; }
ZAHL_INLINE void zneg(z_t a, z_t b)   { ZAHL_SET(a, b); a->sign = -a->sign; }


ZAHL_INLINE void
zswap(z_t a, z_t b)
{
	z_t t;
	t->sign = a->sign;
	a->sign = b->sign;
	b->sign = t->sign;
	t->used = b->used;
	b->used = a->used;
	a->used = t->used;
	t->alloced = a->alloced;
	a->alloced = b->alloced;
	b->alloced = t->alloced;
	t->chars = b->chars;
	b->chars = a->chars;
	a->chars = t->chars;
}


ZAHL_INLINE void
zseti(z_t a, int64_t b)
{
	if (ZAHL_UNLIKELY(b >= 0)) {
		zsetu(a, (uint64_t)b);
	} else {
		zsetu(a, (uint64_t)-b);
		ZAHL_SET_SIGNUM(a, -1);
	}
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
	extern z_t libzahl_tmp_cmp;
	if (ZAHL_UNLIKELY(!b))
		return zsignum(a);
	if (ZAHL_UNLIKELY(zsignum(a) <= 0))
		return -1;
	libzahl_tmp_cmp->chars[0] = b;
	return zcmpmag(a, libzahl_tmp_cmp);
}


ZAHL_INLINE int
zcmpi(z_t a, int64_t b)
{
	extern z_t libzahl_tmp_cmp;
	if (ZAHL_UNLIKELY(!b))
		return zsignum(a);
	if (ZAHL_UNLIKELY(zzero(a)))
		return ZAHL_LIKELY(b < 0) ? 1 : -1;
	if (ZAHL_LIKELY(b < 0)) {
		if (zsignum(a) > 0)
			return +1;
		libzahl_tmp_cmp->chars[0] = (zahl_char_t)-b;
		return -zcmpmag(a, libzahl_tmp_cmp);
	} else {
		if (zsignum(a) < 0)
			return -1;
		libzahl_tmp_cmp->chars[0] = (zahl_char_t)b;
		return +zcmpmag(a, libzahl_tmp_cmp);
	}
}


void zbset_impl_set(z_t a, size_t bit);
void zbset_impl_clear(z_t a, size_t bit);
void zbset_impl_flip(z_t a, size_t bit);

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
		zbset_impl_set(a, bit);
	else if (action < 0)
		zbset_impl_flip(a, bit);
	else
		zbset_impl_clear(a, bit);
}


ZAHL_INLINE int
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
