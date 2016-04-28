#if defined(__linux__)
# define _GNU_SOURCE
# include <sched.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#ifdef BENCHMARK_LIB
# include BENCHMARK_LIB
#else
# include "../zahl.h"
# define BIGINT_LIBRARY "libzahl"
#endif


#ifndef CLOCK_MONOTONIC_RAW
# define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif


#ifdef __x86_64__
# define RDTSC_MAYBE_SUPPORTED
#endif


#if !defined(USE_RDTSC) && !defined(USE_CLOCK) && !defined(USE_GETTIME)
# if 1 && defined(RDTSC_MAYBE_SUPPORTED) && defined(__linux__)
#  define USE_RDTSC
# elif 1
#  define USE_CLOCK
# else
#  define USE_GETTIME
# endif
#endif


static struct timespec dur;
static char timebuf[512];
#if defined(USE_RDTSC)
typedef unsigned long long int rdtsc_t;
static unsigned int start_high, start_low, end_high, end_low;
static unsigned long long int freq;
#elif defined(USE_CLOCK)
static clock_t start, end;
#else
static struct timespec start;
#endif


static void
benchmark_init(void)
{
#if defined(__linux__)
	cpu_set_t cpuset;
# if defined(USE_RDTSC)
	FILE *f;
	char *line = 0;
	size_t size = 0;
# endif
	CPU_ZERO(&cpuset);
	CPU_SET(0, &cpuset);
	sched_setaffinity(getpid(), sizeof(cpuset), &cpuset);
# if defined(USE_RDTSC)
	f = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
	if (getline(&line, &size, f) < 0)
		abort();
	fclose(f);
	freq = strtoull(line, 0, 10);
	free(line);
# endif
#endif
	(void) timebuf;
}


#if defined(USE_RDTSC) && defined(__x86_64__)
static inline void
rdtsc(unsigned int *low, unsigned int *high)
{
	__asm__ __volatile__ ("rdtsc" : "=a"(*low), "=d"(*high));
}
static inline rdtsc_t
rdtsc_join(unsigned int low, unsigned int high)
{
	return (rdtsc_t)low | (((rdtsc_t)high) << 32);
}
#endif


#if defined(USE_RDTSC)
# define TIC  (rdtsc(&start_low, &start_high))
# define TOC\
	do {\
		rdtsc_t dur_cycles;\
		double dur_seconds;\
		rdtsc(&end_low, &end_high);\
		dur_cycles = rdtsc_join(end_low, end_high);\
		dur_cycles -= rdtsc_join(start_low, start_high);\
		dur_seconds = (double)dur_cycles;\
		dur_seconds /= 1000 * (double)freq;\
		dur_seconds -= (double)(dur.tv_sec = (int)dur_seconds);\
		dur.tv_nsec = (long int)(dur_seconds * 1000000000L);\
	} while (0)
#elif defined(USE_CLOCK)
# define TIC  (start = clock())
# define TOC\
	do {\
		end = clock();\
		dur.tv_sec = (end - start) / 1000000ULL;\
		dur.tv_nsec = ((end - start) % 1000000ULL) * 1000;\
	} while (0)
#elif defined(USE_GETTIME)
# define TIC  clock_gettime(CLOCK_MONOTONIC_RAW, &start)
# define TOC\
	do {\
		clock_gettime(CLOCK_MONOTONIC_RAW, &dur);\
		dur.tv_sec -= start.tv_sec;\
		dur.tv_nsec -= start.tv_nsec;\
		if (dur.tv_nsec < 0) {\
			dur.tv_nsec += 1000000000L;\
			dur.tv_sec -= 1;\
		}\
	} while (0)
#endif


#define TICKS  ((unsigned long long int)(dur.tv_sec) * 1000000000ULL + (unsigned long long int)(dur.tv_nsec))
#define STIME  (sprintf(timebuf, "%lli.%09li", (long long)(dur.tv_sec), dur.tv_nsec), timebuf)
