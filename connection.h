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

#include "program_arguments.h"

void setIpAddress(struct in_addr address, char *host);
int createSocket();
void sendData(int sock, char *data, int size, struct sockaddr_in server, socklen_t len);
void getData(int sock, struct sockaddr_in client, socklen_t len);


#endif // CONNECTION_H
