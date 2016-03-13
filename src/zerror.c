/* See LICENSE file for copyright and license details. */
#include "internals.h"


#define LIST_ERRORS\
	X(ZERROR_0_POW_0,  "indeterminate form: 0:th power of 0")\
	X(ZERROR_0_DIV_0,  "indeterminate form: 0 divided by 0")\
	X(ZERROR_DIV_0,    "undefined result: division by 0")\
	X(ZERROR_NEGATIVE, "argument must be non-negative")


enum zerror
zerror(const char **desc)
{
	if (libzahl_error >= 0) {
		if (desc)
			*desc = strerror(libzahl_error);
		errno = libzahl_error;
		return ZERROR_ERRNO_SET;
	}

	if (desc) {
		switch (-libzahl_error) {
#define X(V, D) case V: *desc = D; break;
		LIST_ERRORS
#undef X
		default:
			abort();
		}
	}
	return -libzahl_error;
}
