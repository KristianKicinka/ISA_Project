CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm  -ggdb3
FILES= ../error.c ../program_arguments.c dns_receiver.c 

.PHONY: dns_receiver clean

all:dns_receiver

dns_receiver: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

clean:
	@rm -f dns_receiver
