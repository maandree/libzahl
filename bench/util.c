#define COMPILING_UTIL_C
#include "util.h"


char timebuf[512];
unsigned long long int freq;


void
benchmark_init(void)
{
#if defined(__linux__) && defined(USE_RDTSC)
	cpu_set_t cpuset;
	FILE *f;
	char *line = 0;
	size_t size = 0;
	char path[PATH_MAX];
	CPU_ZERO(&cpuset);
	CPU_SET(USE_CPU, &cpuset);
	sched_setaffinity(getpid(), sizeof(cpuset), &cpuset);
	sprintf(path, "/sys/devices/system/cpu/cpu%i/cpufreq/cpuinfo_max_freq", USE_CPU);
	f = fopen(path, "r");
	if (getline(&line, &size, f) < 0)
		abort();
	fclose(f);
	freq = strtoull(line, 0, 10);
	free(line);

#elif defined(__linux__)
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(USE_CPU, &cpuset);
	sched_setaffinity(getpid(), sizeof(cpuset), &cpuset);

#else
	fprintf(stderr, "WARNING: Don't know how to set CPU affinity.\n");

#endif
}
