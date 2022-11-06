#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h> 

#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <inttypes.h>
#include <errno.h>

#include "program_arguments.h"
#include "base32.h"

//  Štruktúra DNS hlavičky
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

typedef enum {
  INIT_PACKET,
  DATA_PACKET,
} PacketType;

void initSenderDNSheader(unsigned char *buffer, int packet_id, PacketType type);
int createSocket();
void translateToDNSquery(char* query, char *data);
void sendDataToDnsIP(struct sockaddr_in destination, char *base_host, char *dataPayload, int packet_id, PacketType type);
void sendData(int sock, char *data, int size, struct sockaddr_in server, socklen_t len);
int createDNSquery(unsigned char *query, char *payload, char *base_host);

#endif // CONNECTION_H
