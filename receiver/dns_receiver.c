#include "dns_receiver.h"

#define PORT 8080

int main(int argc, char const *argv[]){
    printf("Hello word ISA! from DNS Reciever\n");

    ReceiverArguments *receiverArguments = parseReceiverArguments(argc, (char**) argv);

    printReceiverArguments(receiverArguments);

    struct sockaddr_in receiver;
    memset(&receiver, 0, sizeof(receiver)); 

    setIpAddress(receiver.sin_addr,receiverArguments->BASE_HOST);
    receiver.sin_family = AF_INET;                   
    receiver.sin_port = htons(PORT);

    int recieverLen = sizeof(receiver);

    int sock = createSocket();

    getData(sock, receiver, recieverLen);

    clearReceiverArguments(receiverArguments);

    return 0;
}

