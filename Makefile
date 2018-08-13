CC=gcc
CFLAGS=

.PHONY: all clean install

all: wexec

wexec: main.c
	$(CC) $(CFLAGS) $< -o $@

install: wexec
	install -t /usr/local/bin wexec
	install -t /usr/local/bin wmake

clean:
	rm -f wexec

