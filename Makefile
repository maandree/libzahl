include config.mk

HDR =\
	zahl.h\
	src/internals.h

FUN =\
	zabs\
	zadd\
	zand\
	zbits\
	zbset\
	zbtest\
	zcmp\
	zcmpi\
	zcmpmag\
	zcmpu\
	zdiv\
	zdivmod\
	zfree\
	zgcd\
	zinit\
	zload\
	zlsb\
	zlsh\
	zmod\
	zmodmul\
	zmodpow\
	zmodpowu\
	zmodsqr\
	zmul\
	zneg\
	znot\
	zor\
	zpow\
	zpowu\
	zptest\
	zrand\
	zrsh\
	zsave\
	zset\
	zseti\
	zsets\
	zsetu\
	zsetup\
	zsplit\
	zsqr\
	zstr\
	zstr_length\
	zsub\
	zswap\
	ztrunc\
	zunsetup\
	zxor

OBJ = $(FUN:=.o)
MAN = $(foreach F,$(FUN),man/$(F).3) man/libzahl.7

all: libzahl.a

%.o: src/%.c $(HDR) config.mk
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

libzahl.a: $(OBJ)
	$(AR) rc $@ $?
	$(RANLIB) $@

clean:
	-rm -- *.o *.su *.a *.so 2>/dev/null

.PHONY: all clean
