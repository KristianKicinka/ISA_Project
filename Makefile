CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm  -ggdb3
RECEIVER_FILES= error.c program_arguments.c connection.c ./receiver/dns_receiver.c
SENDER_FILES= error.c program_arguments.c connection.c ./sender/dns_sender.c 

.PHONY: receiver sender clean

all:receiver sender

receiver: $(RECEIVER_FILES)
	$(CC) $(CFLAGS) -o dns_receiver $(RECEIVER_FILES)

sender: $(SENDER_FILES)
	$(CC) $(CFLAGS) -o dns_sender $(SENDER_FILES)

clean:
	@rm -f receiver
	@rm -f sender
