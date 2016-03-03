/* See LICENSE file for copyright and license details. */
#include "../zahl.h"

#define BITS_PER_CHAR                32
#define LB_BITS_PER_CHAR             5
#define ZAHL_CHAR_MAX                UINT32_MAX

#define FLOOR_BITS_TO_CHARS(bits)    ((bits) >> LB_BITS_PER_CHAR)
#define CEILING_BITS_TO_CHARS(bits)  (((bits) + (BITS_PER_CHAR - 1)) >> LB_BITS_PER_CHAR)
#define BITS_IN_LAST_CHAR(bits)      ((bits) & (BITS_PER_CHAR - 1))

#define LIST_TEMPS\
	X(libzahl_tmp_cmp)\
	X(libzahl_tmp_str_num)\
	X(libzahl_tmp_str_mag)\
	X(libzahl_tmp_str_div)\
	X(libzahl_tmp_str_rem)\
	X(libzahl_tmp_gcd_u)\
	X(libzahl_tmp_gcd_v)\
	X(libzahl_tmp_modmul)\
	X(libzahl_tmp_div)\
	X(libzahl_tmp_mod)\
	X(libzahl_tmp_pow_b)\
	X(libzahl_tmp_pow_c)\
	X(libzahl_tmp_pow_d)\
	X(libzahl_tmp_modsqr)\
	X(libzahl_tmp_divmod_a)\
	X(libzahl_tmp_divmod_b)\
	X(libzahl_tmp_divmod_d)\
	X(libzahl_tmp_divmod_e)

#define LIST_CONSTS\
	X(libzahl_const_1e19, zsetu, 10000000000000000000ULL) /* The largest power of 10 < 2⁶⁴. */\
	X(libzahl_const_1e9,  zsetu, 1000000000ULL)           /* The largest power of 10 < 2³². */

#define X(x)  extern z_t x;
LIST_TEMPS
#undef X
#define X(x, f, v)  extern z_t x;
LIST_CONSTS
#undef X

extern jmp_buf libzahl_jmp_buf;
extern int libzahl_set_up;

#define FAILURE_JUMP()  (longjmp(libzahl_jmp_buf, 1))

#define SET_SIGNUM(a, signum)  ((a)->sign = (signum))
