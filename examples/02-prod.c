/* Calculates the product of $@ */

#include <stdio.h>
#include <stdlib.h>

#include <zahl.h>

int
main(int argc, char *argv[])
{
	z_t prod, factor;
	jmp_buf env;
	char *buf;
	int i;

	if (setjmp(env))
		return zperror(argv[0]), 1;

	zsetup(env);
	zinit(prod);
	zinit(factor);
	zsetu(prod, 1);

	for (i = 1; i < argc; i++) {
		zsets(factor, argv[i]);
		zmul(prod, prod, factor);
	}

	printf("%s\n", buf = zstr(prod, NULL, 0));
	free(buf);

	zfree(factor);
	zfree(prod);
	zunsetup();
	return 0;
}
