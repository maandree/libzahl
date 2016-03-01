/* See LICENSE file for copyright and license details. */
#include "../zahl.h"

#define BITS_PER_CHAR                32
#define LB_BITS_PER_CHAR             5

#define FLOOR_BITS_TO_CHARS(bits)    ((bits) >> LB_BITS_PER_CHAR)
#define CEILING_BITS_TO_CHARS(bits)  (((bits) + (BITS_PER_CHAR - 1)) >> LB_BITS_PER_CHAR)
#define BITS_IN_LAST_CHAR(bits)      ((bits) & (BITS_PER_CHAR - 1))

#define LIST_TEMPS\
	X(libzahl_tmp_cmp)

#define X(x)  extern z_t x;
LIST_TEMPS
#undef X

extern jmp_buf libzahl_jmp_buf;
extern int libzahl_set_up;

#define FAILURE_JUMP()  (longjmp(libzahl_jmp_buf, 1))

#define SET_SIGNUM(a, signum)  ((a)->sign = (signum))
