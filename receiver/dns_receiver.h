#ifndef DNS_RECEIVER_H
#define DNS_RECEIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../connection.h"

/*
typedef struct receiver_data_t {
    FILE *file,
} ReceiverData;
*/

void createFile(FILE *file, char *receiver_part, char *sender_part);
void closeFile(FILE *file);
void writeToFile(FILE *file, char *data);
void sendConfirmFilePath(int socket, struct sockaddr_in destination);
void sendConfirmData(int socket, struct sockaddr_in destination);
void sendEndTransport(int socket, struct sockaddr_in destination);


#endif // DNS_RECEIVER_H
