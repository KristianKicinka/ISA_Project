/**
 * @file dns_receiver.h
 * @author Kristián Kičinka (xkicin02)
 * @brief 
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef DNS_RECEIVER_H
#define DNS_RECEIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../connection.h"
#include "dns_receiver_events.h"

void writeToFile(char *path, char *data, int data_size);
void proccessDataPayload(char *data_payload, ReceiverArguments *arguments, struct in_addr *address, PacketType type);
void sendConfirmPacket(int socket, struct sockaddr_in destination, char *recv_packet);
void convertDNSQuery(char *data);
int getDataFromPayload(char *data_payload, unsigned char *data, int data_size, char *base_host, PacketType type);
void callParsedQuery(char *data_part, char* base_host);


#endif // DNS_RECEIVER_H
