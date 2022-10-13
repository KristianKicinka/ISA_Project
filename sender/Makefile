CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm  -ggdb3
FILES= ../error.c ../program_arguments.c dns_sender.c

.PHONY: dns_sender clean

all:dns_sender

dns_sender: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

clean:
	@rm -f dns_sender
