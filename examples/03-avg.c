/* Calculates the truncated average of $@ */

#include <stdio.h>
#include <stdlib.h>

#include <zahl.h>

int
main(int argc, char *argv[])
{
	z_t sum, term;
	jmp_buf env;
	char *buf;
	int i;

	if (setjmp(env))
		return zperror(argv[0]), 1;

	zsetup(env);
	zinit(sum);
	zinit(term);
	zsetu(sum, 0);

	for (i = 1; i < argc; i++) {
		zsets(term, argv[i]);
		zadd(sum, sum, term);
	}
	zseti(term, argc);
	zdiv(sum, sum, term);

	printf("%s\n", buf = zstr(sum, NULL, 0));
	free(buf);

	zfree(term);
	zfree(sum);
	zunsetup();
	return 0;
}
