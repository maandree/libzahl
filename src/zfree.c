/* See LICENSE file for copyright and license details. */
#include "internals.h"


void
zfree(z_t a)
{
	free(a->chars);
}
