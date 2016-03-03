/* See LICENSE file for copyright and license details. */
#include "internals"


void
zfree(z_t a)
{
	free(a->chars);
}
