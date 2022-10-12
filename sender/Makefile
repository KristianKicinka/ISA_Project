CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm  -ggdb3
FILES= dns_sender.c

.PHONY: build clean

all:build

build: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

clean:
	@rm -f build
