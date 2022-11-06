#ifndef DNS_RECEIVER_H
#define DNS_RECEIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../connection.h"

void writeToFile(char *path, char *data);
void proccessDataPayload(char *data_payload, ReceiverArguments *arguments, PacketType type);
void sendConfirmPacket(int socket, struct sockaddr_in destination, char *recv_packet);
void convertDNSQuery(char *data);

void getDataFromPayload(char *data_payload, unsigned char *data, int data_size);


#endif // DNS_RECEIVER_H
