/* See LICENSE file for copyright and license details. */
#include "../zahl.h"

#define BITS_PER_CHAR                32
#define FLOOR_BITS_TO_CHARS(bits)    ((bits) >> 5)
#define CEILING_BITS_TO_CHARS(bits)  (((bits) + (BITS_PER_CHAR - 1)) >> 5)
#define BITS_IN_LAST_CHAR(bits)      ((bits) & (BITS_PER_CHAR - 1))

#define LIST_TEMPS\
	X(libzahl_tmp_a)\
	X(libzahl_tmp_b)\
	X(libzahl_tmp_c)\
	X(libzahl_tmp_d)\
	X(libzahl_tmp_e)\
	X(libzahl_tmp_f)\
	X(libzahl_tmp_g)\
	X(libzahl_tmp_i)\
	X(libzahl_tmp_j)\
	X(libzahl_tmp_k)\
	X(libzahl_tmp_l)\
	X(libzahl_tmp_m)

#define X(x)  extern z_t x;
LIST_TEMPS
#undef X

extern jmp_buf libzahl_jmp_buf;
extern int libzahl_set_up;

#define FAILURE_JUMP()  (longjmp(libzahl_jmp_buf, 1))
