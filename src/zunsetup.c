/* See LICENSE file for copyright and license details. */
#include "internals"


void
zunsetup(jmp_buf env)
{
	size_t i;
	if (libzahl_set_up) {
		libzahl_set_up = 0;
#define X(x)\
		zfree(x);
		LIST_TEMPS;
#undef X
#define X(x, f, v)\
		zfree(x);
		LIST_CONSTS;
#undef X
		for (i = BITS_PER_CHAR; i--;)
			zfree(libzahl_tmp_divmod_ds[i]);
	}
}
