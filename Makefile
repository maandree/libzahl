include config.mk

HDR_PUBLIC =\
	zahl.h\
	zahl-internals.h\
	zahl-inlines.h

HDR_PRIVATE =\
	src/internals.h

FUN =\
	zadd\
	zand\
	zbset\
	zdiv\
	zdivmod\
	zerror\
	zfree\
	zgcd\
	zload\
	zlsh\
	zmod\
	zmodmul\
	zmodpow\
	zmodpowu\
	zmodsqr\
	zmul\
	znot\
	zor\
	zperror\
	zpow\
	zpowu\
	zptest\
	zrand\
	zrsh\
	zsave\
	zset\
	zsets\
	zsetu\
	zsetup\
	zsplit\
	zsqr\
	zstr\
	zstr_length\
	zsub\
	ztrunc\
	zunsetup\
	zxor

INLINE_FUN =\
	zinit\
	zswap\
	zeven\
	zodd\
	zeven_nonzero\
	zodd_nonzero\
	zzero\
	zsignum\
	zabs\
	zneg\
	zlsb\
	zbits\
	zseti\
	zcmp\
	zcmpi\
	zcmpmag\
	zcmpu\
	zbtest

HDR  = $(HDR_PUBLIC) $(HDR_PRIVATE)
OBJ  = $(FUN:=.o) allocator.o
MAN3 = $(FUN:=.3) $(INLINE_FUN:=.3)
MAN7 = libzahl.7

VPATH = src

BENCHMARK_LIB_            = libzahl.a
BENCHMARK_LIB_zahl        = libzahl.a
BENCHMARK_LIB_libzahl     = libzahl.a
BENCHMARK_LIB_tommath     = -ltommath
BENCHMARK_LIB_libtommath  = -ltommath
BENCHMARK_LIB_gmp         = -lgmp
BENCHMARK_LIB_libgmp      = -lgmp
BENCHMARK_LIB_tfm         = libtfm.a
BENCHMARK_LIB_libtfm      = libtfm.a
BENCHMARK_LIB_hebimath    = libhebimath.a
BENCHMARK_LIB_libhebimath = libhebimath.a

BENCHMARK_DEP_            = libzahl.a
BENCHMARK_DEP_zahl        = libzahl.a
BENCHMARK_DEP_libzahl     = libzahl.a
BENCHMARK_DEP_tommath     = bench/libtommath.h
BENCHMARK_DEP_libtommath  = bench/libtommath.h
BENCHMARK_DEP_gmp         = bench/libgmp.h
BENCHMARK_DEP_libgmp      = bench/libgmp.h
BENCHMARK_DEP_tfm         = bench/libtfm.h
BENCHMARK_DEP_libtfm      = bench/libtfm.h
BENCHMARK_DEP_hebimath    = bench/libhebimath.h
BENCHMARK_DEP_libhebimath = bench/libhebimath.h

BENCHMARK_CPP_tommath     = '-DBENCHMARK_LIB="libtommath.h"'
BENCHMARK_CPP_libtommath  = '-DBENCHMARK_LIB="libtommath.h"'
BENCHMARK_CPP_gmp         = '-DBENCHMARK_LIB="libgmp.h"'
BENCHMARK_CPP_libgmp      = '-DBENCHMARK_LIB="libgmp.h"'
BENCHMARK_CPP_tfm         = '-DBENCHMARK_LIB="libtfm.h"'
BENCHMARK_CPP_libtfm      = '-DBENCHMARK_LIB="libtfm.h"'
BENCHMARK_CPP_hebimath    = '-DBENCHMARK_LIB="libhebimath.h"'
BENCHMARK_CPP_libhebimath = '-DBENCHMARK_LIB="libhebimath.h"'

CPPFLAGS += $(BENCHMARK_CPP_$(BENCHMARK_LIB))

CFLAGS_WITHOUT_O = $$(printf '%s\n' $(CFLAGS) | sed '/^-O.*$$/d')


all: libzahl.a

.o: .c $(HDR) config.mk
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

libzahl.a: $(OBJ)
	$(AR) rc $@ $?
	$(RANLIB) $@

test-random.c: test-generate.py
	./test-generate.py > test-random.c

test: test.c libzahl.a test-random.c
	$(CC) $(LDFLAGS) $(CFLAGS_WITHOUT_O) -O0 $(CPPFLAGS) -o $@ test.c libzahl.a

benchmark: bench/benchmark.c $(BENCHMARK_DEP_$(BENCHMARK_LIB))
	$(CC) $(LDFLAGS) $(CFLAGS) $(CPPFLAGS) -o $@ bench/benchmark.c $(BENCHMARK_LIB_$(BENCHMARK_LIB))

benchmark-zrand: bench/benchmark-zrand.c libzahl.a
	$(CC) $(LDFLAGS) $(CFLAGS) $(CPPFLAGS) -o $@ $^

check: test
	./test

install: libzahl.a
	mkdir -p -- "$(DESTDIR)$(EXECPREFIX)/lib"
	mkdir -p -- "$(DESTDIR)$(PREFIX)/include"
	mkdir -p -- "$(DESTDIR)$(MANPREFIX)/man3"
	mkdir -p -- "$(DESTDIR)$(MANPREFIX)/man7"
	@if test -n "$(DESTDIR)"; then \
		cd man && test -d "$(DESTDIR)$(MANPREFIX)/man7" || \
		(printf '\n\n!!  DESTDIR must be an absolute path.  !!\n\n\n' ; exit 1) \
	fi
	cp -- libzahl.a "$(DESTDIR)$(EXECPREFIX)/lib"
	cp -- $(HDR_PUBLIC) "$(DESTDIR)$(PREFIX)/include"
	cd man && cp -- $(MAN3) "$(DESTDIR)$(MANPREFIX)/man3"
	cd man && cp -- $(MAN7) "$(DESTDIR)$(MANPREFIX)/man7"

uninstall:
	-rm -- "$(DESTDIR)$(EXECPREFIX)/lib/libzahl.a"
	-cd -- "$(DESTDIR)$(PREFIX)/include" && rm $(HDR_PUBLIC)
	-cd -- "$(DESTDIR)$(MANPREFIX)/man3" && rm $(MAN3)
	-cd -- "$(DESTDIR)$(MANPREFIX)/man7" && rm $(MAN7)

clean:
	-rm -- *.o *.su *.a *.so test test-random.c 2>/dev/null

.PHONY: all check clean install uninstall
