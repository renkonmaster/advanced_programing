CC = gcc
CFLAGS = -Wall -Wextra
SOURCES = $(wildcard *.c)
TARGETS = $(SOURCES:.c=)

all:  $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY:	all clean
