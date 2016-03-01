/* See LICENSE file for copyright and license details. */
#include "internals"

#define X(x)  z_t x;
LIST_TEMPS
#undef X

jmp_buf libzahl_jmp_buf;
int libzahl_set_up = 0;


void
zsetup(jmp_buf env)
{
	libzahl_jmp_buf = jmp_buf;

	if (!libzahl_set_up) {
		libzahl_set_up = 1;
#define X(x)  zinit(zahl_tmp_##x);
		LIST_TEMPS;
#undef X
	}
}
