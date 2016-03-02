/* See LICENSE file for copyright and license details. */
#include "internals"

#define X(x)  z_t x;
LIST_TEMPS
#undef X
#define X(x, f, v)  z_t x;
LIST_CONSTS
#undef X

jmp_buf libzahl_jmp_buf;
int libzahl_set_up = 0;


void
zsetup(jmp_buf env)
{
	libzahl_jmp_buf = jmp_buf;

	if (!libzahl_set_up) {
		libzahl_set_up = 1;
#define X(x)\
		zinit(x);
		LIST_TEMPS;
#undef X
#define X(x, f, v)\
		zinit(x), f(x, v);
		LIST_CONSTS;
#undef X
	}
}
