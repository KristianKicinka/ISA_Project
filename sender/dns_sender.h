/**
 * @file dns_sender.h
 * @author Kristián Kičinka (xkicin02)
 * @brief 
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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
void sendSenderData(SenderArguments *senderArguments, char *dataPayload, PacketType type);
void sendInitPacket(char *ip_address, char *data, char *base_host);
void sendDataPacket(char *ip_address, char *data, char *base_host);
void loadData(SenderArguments *senderArguments);
char *getImplicitDNSserverIP();




#endif // DNS_SENDER_H
