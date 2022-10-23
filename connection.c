#include "connection.h"

void setIpAddress(struct in_addr address, char *host){
    struct hostent *hostent;

    if ((hostent = gethostbyname(host)) == NULL)
        proccessError(INTERNAL_ERROR);
    
    memcpy(&address, hostent->h_addr, hostent->h_length); 
}

int createSocket(){
    int sock;
    if ((sock = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP)) == -1)
        proccessError(INTERNAL_ERROR);
    return sock;
}

void sendData(int sock, char *data, int size, struct sockaddr_in server, socklen_t len){

    int sendResult = sendto(sock, data, size, 0, (struct sockaddr *) &server, len);

    if (sendResult == -1)
        proccessError(INTERNAL_ERROR);
    else if (sendResult != size)
        proccessError(INTERNAL_ERROR); // partially send data
}

void getData(int sock, struct sockaddr_in client, socklen_t len){
    char data[1024];
    int getResult;

    while ((getResult = recvfrom(sock, data, 1024, 0, (struct sockaddr *) &client, &len)) >= 0){
        if(getResult == -1)   
            proccessError(INTERNAL_ERROR);
        else if (getResult > 0){
            printf("* UDP packet received from %s, port %d (%d)\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port), client.sin_port);
            printf("%.*s", getResult, data);
        }
    }
}
