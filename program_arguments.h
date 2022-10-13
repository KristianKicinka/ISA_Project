#ifndef PROGRAM_ARGUMENTS_H
#define PROGRAM_ARGUMENTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

typedef struct sender_arguments_t {
    char *UPSTREAM_DNS_IP;
    char *BASE_HOST;
    char *DST_FILEPATH;
    char *SRC_FILEPATH;
}SenderArguments;

typedef struct receiver_arguments_t {
    char *BASE_HOST;
    char *DST_FILEPATH;
}ReceiverArguments;

void setArgument(char **argument, char *value);

SenderArguments *initSenderArguments();
SenderArguments *parseSenderArguments(int argc, char *argv[]);
void clearSenderArguments(SenderArguments *arguments);
void printSenderArguments(SenderArguments *senderArguments);

ReceiverArguments *initReceiverArguments();
ReceiverArguments *parseReceiverArguments(int argc, char *argv[]);
void clearReceiverArguments(ReceiverArguments *arguments);
void printReceiverArguments(ReceiverArguments *receiverArguments);



#endif // PROGRAM_ARGUMENTS_H
