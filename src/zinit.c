/* See LICENSE file for copyright and license details. */
#include "internals"


void
zinit(z_t a)
{
	a->alloced = 0;
	a->chars = 0;
}
