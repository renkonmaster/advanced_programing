CC = gcc
CFLAGS = -Wall -Wextra
SOURCES = $(wildcard *.c)
TARGETS = $(SOURCES:.c=)

all:  $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

ex14-6-fast: ex14-6-fast.c hugeint_fast.c hugeint_fast.h
	$(CC) $(CFLAGS) -std=c17 -O3 -DNDEBUG -o $@ ex14-6-fast.c hugeint_fast.c $(LDFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY:	all clean
