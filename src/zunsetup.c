/* See LICENSE file for copyright and license details. */
#include "internals"


void
zunsetup(jmp_buf env)
{
	if (libzahl_set_up) {
		libzahl_set_up = 0;
#define X(x)  zfree(zahl_tmp_##x);
		LIST_TEMPS;
#undef X
	}
}
