/* See LICENSE file for copyright and license details. */
#include "internals.h"

#define X(x, s)  z_t x;
LIST_TEMPS
#undef X
#define X(i, x, f, v)  z_t x;
LIST_CONSTS
#undef X

z_t libzahl_tmp_divmod_ds[BITS_PER_CHAR];
jmp_buf libzahl_jmp_buf;
int libzahl_set_up = 0;
int libzahl_error;
zahl_char_t **libzahl_pool[sizeof(size_t) * 8];
size_t libzahl_pool_n[sizeof(size_t) * 8];
size_t libzahl_pool_alloc[sizeof(size_t) * 8];
struct zahl **libzahl_temp_stack;
struct zahl **libzahl_temp_stack_head;
struct zahl **libzahl_temp_stack_end;
void *libzahl_temp_allocation = 0;

#define X(i, x, f, v)  1 +
static zahl_char_t constant_chars[LIST_CONSTS ZAHL_FLUFF];
#undef X


void
zsetup(jmp_buf env)
{
	size_t i;
	*libzahl_jmp_buf = *env;

	if (likely(!libzahl_set_up)) {
		libzahl_set_up = 1;

		memset(libzahl_pool, 0, sizeof(libzahl_pool));
		memset(libzahl_pool_n, 0, sizeof(libzahl_pool_n));
		memset(libzahl_pool_alloc, 0, sizeof(libzahl_pool_alloc));

#define X(x, s)\
		zinit(x); if (s) zsetu(x, 1);
		LIST_TEMPS;
#undef X
#define X(i, x, f, v)\
		(x)->alloced = 1, (x)->chars = constant_chars + (i), f(x, v);
		LIST_CONSTS;
#undef X
		for (i = BITS_PER_CHAR; i--;)
			zinit(libzahl_tmp_divmod_ds[i]);

		libzahl_temp_stack = malloc(256 * sizeof(*libzahl_temp_stack));
		if (check(!libzahl_temp_stack))
			libzahl_memfailure();
		libzahl_temp_stack_head = libzahl_temp_stack;
		libzahl_temp_stack_end = libzahl_temp_stack + 256;
	}
}
