/* See LICENSE file for copyright and license details. */
#include "internals.h"

#include <stdio.h>


void
zperror(const char *prefix)
{
	if (libzahl_error >= 0) {
		errno = libzahl_error;
		perror(prefix);
	} else {
		const char *desc;
		zerror(&desc);
		if (prefix && *prefix)
			fprintf(stderr, "%s: %s\n", prefix, desc);
		else
			fprintf(stderr, "%s\n", desc);
	}
}
