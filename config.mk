VERSION = 1.1

PREFIX = /usr/local
EXECPREFIX = $(PREFIX)
MANPREFIX = $(PREFIX)/share/man
DOCPREFIX = $(PREFIX)/share/doc

CC = cc
AR = ar
RANLIB = ranlib

# Unless /dev/urandom exists and is a non-blocking random number generator,
# you have to add -DFAST_RANDOM_PATHNAME=... to CPPFLAGS, and
# unless /dev/random exists and is a blocking secure random number generator
# you have to add -DSECURE_RANDOM_PATHNAME=... to CPPFLAGS.
# Remove -DGOOD_RAND if the higher bits have higher entropy in the lower
# bits in rand(3), this was historically the case.
# Add -DUNSAFE if you rather libzahl did not check for errors.

CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -DGOOD_RAND
CFLAGS   = -std=c99 -flto -O3 -Wall -pedantic
LDFLAGS  = -s
