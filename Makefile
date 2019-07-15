CC=gcc
CFLAGS=

.PHONY: all clean install

all: wexec

wexec: main.c
	$(CC) $(CFLAGS) $< -o $@

install: wexec
	install -t $(DESTDIR)/bin wexec
	install -t $(DESTDIR)/bin wmake

clean:
	rm -f wexec
