#include "dns_receiver.h"

//#define PACKET_ID 1489
#define DNS_PORT 53
#define DNS_PACKET_LEN 512
#define FILE_PATH_LEN 1024 

int main(int argc, char const *argv[]){
    printf("Hello word ISA! from DNS Reciever\n");

    ReceiverArguments *receiverArguments = parseReceiverArguments(argc, (char**) argv);

    printReceiverArguments(receiverArguments);

    //int sock = createSocket();
    printf("* Opening UDP socket ...\n");
    int sock, result;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) // create the server UDP socket
        proccessError(INTERNAL_ERROR);
    
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        proccessError(INTERNAL_ERROR);

    socklen_t length;
    char recv_buffer[DNS_PACKET_LEN] = {0};
    struct sockaddr_in receiver;
    memset(&receiver, 0, sizeof(receiver));

    receiver.sin_family = AF_INET;     
    receiver.sin_addr.s_addr = htonl(INADDR_ANY);              
    receiver.sin_port =  htons(DNS_PORT);
    

    if (bind(sock, (struct sockaddr *)&receiver, sizeof(receiver)) == -1) // binding with the port
        proccessError(INTERNAL_ERROR);

    length = sizeof(receiver);

    printf("Pred while\n");
    while ((result = recvfrom(sock, recv_buffer, DNS_PACKET_LEN, 0, (struct sockaddr *) &receiver, &length)) >= 0){
        printf("Vo while\n");
        if(result == -1)   
            proccessError(INTERNAL_ERROR);
        
        printf("* UDP packet received from %s, port %d (%d)\n",inet_ntoa(receiver.sin_addr),ntohs(receiver.sin_port), receiver.sin_port);
        printf("%.*s", result, recv_buffer);
    }
    printf("za while\n");
    clearReceiverArguments(receiverArguments);

    return 0;
}

void createFile(FILE *file, char *receiver_part, char *sender_part){

    char file_path[FILE_PATH_LEN];
    strcpy(file_path, receiver_part);
    strcpy(file_path, sender_part);

    if((file = fopen (file_path, "w")) == NULL)
        proccessError(INTERNAL_ERROR);

}

void closeFile(FILE *file){
    fclose(file);
}

void writeToFile(FILE *file, char *data){
    fprintf(file,"%s",data);
}

void sendConfirmFilePath(int socket, struct sockaddr_in destination){
    char buffer[DNS_PACKET_LEN] = {0};

}

void sendConfirmData(int socket, struct sockaddr_in destination){
    char buffer[DNS_PACKET_LEN] = {0};

}

void sendEndTransport(int socket, struct sockaddr_in destination){
    char buffer[DNS_PACKET_LEN] = {0};

}
