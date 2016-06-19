/* Calculates the median of $@ */

#include <stdio.h>
#include <stdlib.h>

#include <zahl.h>

int
main(int argc, char *argv[])
{
	struct zahl *values;
	z_t med, medmod;
	jmp_buf env;
	char *buf, *argv0;
	int i, j;

	argv0 = *argv++, argc--;

	if (!argc) {
		fprintf(stderr,
		        "%s: cannot calculate median of the empty bag\n",
		        argv0);
		return 1;
	}

	values = calloc(argc, sizeof(*values));
	if (!values)
		return perror(argv0), 1;

	if (setjmp(env))
		return zperror(argv0), 1;

	zsetup(env);
	zinit(med);
	zinit(medmod);

	/* Since `values` where allocated with
	 * `calloc` it is already cleared and
	 * `zinit` is not necessary. */

	for (i = 0; i < argc; i++)
		zsets(&values[i], argv[i]);

	qsort(values, argc, sizeof(*values),
	      (int (*)(const void *, const void *))zcmp);
	i = argc / 2;
	j = i - !(argc & 1);
	zadd(med, &values[i], &values[j]);
	zsetu(medmod, 2);
	zdivmod(med, medmod, med, medmod);

	printf("%s%s\n", buf = zstr(med, NULL, 0),
	               (const char *[]){"", ".5"}[zodd(medmod)]);
	free(buf);

	zfree(medmod);
	zfree(med);
	for (i = 0; i < argc; i++)
		zfree(&values[i]);
	free(values);
	zunsetup();
	return 0;
}
