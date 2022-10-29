#ifndef DNS_SENDER_H
#define DNS_SENDER_H

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h> 

#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>

#include "../connection.h"

typedef struct senderData_t {
    int socket;
}SenderData;

SenderData *initSenderData();
void clearSenderData(SenderData *senderData);
void sendSenderData(SenderArguments *senderArguments, char *dataPayload);
void loadData(SenderArguments *senderArguments);
char *getImplicitDNSserverIP();




#endif // DNS_SENDER_H
