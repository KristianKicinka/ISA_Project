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

//DNS header structure
// All uint16 are in network byte order!!!
typedef struct __attribute__((__packed__)) dns_header {
  uint16_t id;

  unsigned int rd : 1;
  unsigned int tc : 1;
  unsigned int aa : 1;
  unsigned int opcode : 4;
  unsigned int qr : 1;

  unsigned int rcode : 4;
  unsigned int z : 3;
  unsigned int ra : 1;

  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;
} DnsHeader;

typedef struct senderData_t {
    int socket;
}SenderData;

SenderData *initSenderData();
void clearSenderData(SenderData *senderData);

void sendDnsData(SenderArguments *senderArguments, char *dataPayload);
void sendDataToDnsIP(SenderArguments *senderArguments, char *dataPayload);
void loadData(SenderArguments *senderArguments);
char *encodeData(char* data);
void initDnsHeader(unsigned char *buffer);
void translateToDNSquery(char* query, char *data);
int createDNSquery(unsigned char *query, char *payload, char *base_host);




#endif // DNS_SENDER_H
