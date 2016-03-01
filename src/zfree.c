/* See LICENSE file for copyright and license details. */
#include "internals"

#include <stdlib.h>


void
zfree(z_t a)
{
	free(a->chars);
	a->alloced = 0;
	a->chars = 0;
}
