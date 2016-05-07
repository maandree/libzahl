/* See LICENSE file for copyright and license details. */

#define LIBZAHL_MEM_CASES  \
	LIBZAHL_X(20);     \
	LIBZAHL_X(19);     \
	LIBZAHL_X(18);     \
	LIBZAHL_X(17);     \
	LIBZAHL_X(16);     \
	LIBZAHL_X(15);     \
	LIBZAHL_X(14);     \
	LIBZAHL_X(13);     \
	LIBZAHL_X(12);     \
	LIBZAHL_X(11);     \
	LIBZAHL_X(10);     \
	LIBZAHL_X( 9);     \
	LIBZAHL_X( 8);     \
	LIBZAHL_X( 7);     \
	LIBZAHL_X( 6);     \
	LIBZAHL_X( 5);     \
	LIBZAHL_X( 4);     \
	LIBZAHL_X( 3);     \
	LIBZAHL_X( 2);     \
	LIBZAHL_X( 1);     \
	case 0: break;


#if defined(ZAHL_ISA_MISSING_INDIRECT_JUMP)
# define LIBZAHL_SMALL_INPUT_BEGIN(n)
# define LIBZAHL_SMALL_INPUT_END
#else
# define LIBZAHL_SMALL_INPUT_BEGIN(n)  switch (n) { LIBZAHL_MEM_CASES default:
# define LIBZAHL_SMALL_INPUT_END       break; }
#endif


ZAHL_INLINE void
libzahl_memcpy(register zahl_char_t *restrict d, register const zahl_char_t *restrict s, size_t n)
{
	size_t i;
#define LIBZAHL_X(I)  case I:  d[I - 1] = s[I - 1];
	LIBZAHL_SMALL_INPUT_BEGIN(n);
	for (i = 0; i < n; i += 4) {
		d[i + 0] = s[i + 0];
		d[i + 1] = s[i + 1];
		d[i + 2] = s[i + 2];
		d[i + 3] = s[i + 3];
	}
	LIBZAHL_SMALL_INPUT_END;
#undef LIBZAHL_X
}


ZAHL_INLINE void
libzahl_memset(register zahl_char_t *a, register zahl_char_t v, size_t n)
{
	size_t i;
	for (i = 0; i < n; i += 4) {
		a[i + 0] = v;
		a[i + 1] = v;
		a[i + 2] = v;
		a[i + 3] = v;
	}
}

ZAHL_INLINE void
libzahl_memset_precise(register zahl_char_t *a, register zahl_char_t v, size_t n)
{
	size_t i;
	if (n <= 4) {
		if (n >= 1)
			a[0] = v;
		if (n >= 2)
			a[1] = v;
		if (n >= 3)
			a[2] = v;
		if (n >= 4)
			a[3] = v;
	} else {
		for (i = 0; (i += 4) <= n;) {
			a[i - 1] = v;
			a[i - 2] = v;
			a[i - 3] = v;
			a[i - 4] = v;
		}
		if (i > n)
			for (i -= 4; i < n; i++)
				a[i] = v;
	}
}


ZAHL_INLINE void
libzahl_memmovef(register zahl_char_t *d, register const zahl_char_t *s, size_t n)
{
	if (n && n < 4) {
		d[0] = s[0];
		d[1] = s[1];
		d[2] = s[2];
	} else {
		size_t i;
		for (i = 0; i < n; i += 4) {
			d[i + 0] = s[i + 0];
			d[i + 1] = s[i + 1];
			d[i + 2] = s[i + 2];
			d[i + 3] = s[i + 3];
		}
	}
}

ZAHL_INLINE void
libzahl_memmoveb(register zahl_char_t *d, register const zahl_char_t *s, size_t n)
{
	ssize_t i;
#define LIBZAHL_X(I)  case I:  d[I - 1] = s[I - 1];
	LIBZAHL_SMALL_INPUT_BEGIN(n);
	for (i = ((ssize_t)n + 3) & ~3; (i -= 4) >= 0;) {
		d[i + 3] = s[i + 3];
		d[i + 2] = s[i + 2];
		d[i + 1] = s[i + 1];
		d[i + 0] = s[i + 0];
	}
	LIBZAHL_SMALL_INPUT_END;
#undef LIBZAHL_X
}

ZAHL_INLINE void
libzahl_memmove(register zahl_char_t *d, register const zahl_char_t *s, size_t n)
{
	if (d < s)
		libzahl_memmovef(d, s, n);
	else
		libzahl_memmoveb(d, s, n);
}
