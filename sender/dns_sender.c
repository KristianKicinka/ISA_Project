#include "dns_sender.h"

#define PORT 8080
#define DNS_PAYLOAD_LEN 512

int main(int argc, char const *argv[]){
    printf("Hello word ISA! from DNS Sender\n");

    SenderArguments *senderArguments = parseSenderArguments(argc, (char**) argv);

    printSenderArguments(senderArguments);

    struct sockaddr_in sender;
    memset(&sender, 0, sizeof(sender)); 

    /*
    setIpAddress( sender.sin_addr, senderArguments->BASE_HOST);
    sender.sin_family = AF_INET;                   
    sender.sin_port = htons(PORT);

    int senderLen = sizeof(sender);

    int sock = createSocket();
    char *data = "help";

    sendData(sock, data, strlen(data), sender, senderLen);
    */

    //generateData(senderArguments);
    loadData(senderArguments);

    clearSenderArguments(senderArguments);

    return 0;
}

SenderData *initSenderData(){
    SenderData *senderData = calloc(1, sizeof(SenderData));
    if (senderData == NULL){
        proccessError(INTERNAL_ERROR);
    }
    return senderData;
}

void clearSenderData(SenderData *senderData){
    if (senderData !=NULL){
        free(senderData);
    }    
}

void sendDnsData(SenderArguments *senderArguments, char *dataPayload){
    struct hostent *hostent;
    char data[DNS_PAYLOAD_LEN] = {0};

    dataPayload = encodeData(dataPayload);

    strcat(data, dataPayload);
    strcat(data, ".");
    strcat(data, senderArguments->BASE_HOST);

    if ((hostent = gethostbyname(data)) == NULL)
        proccessError(INTERNAL_ERROR);
}

void loadData(SenderArguments *senderArguments){

    char buffer[DNS_PAYLOAD_LEN];
    FILE* filePointer;

    if (senderArguments->SRC_FILEPATH == NULL)
        filePointer = stdin;
    else{
        filePointer = fopen(senderArguments->SRC_FILEPATH, "r");
        if (filePointer == NULL)
            proccessError(INTERNAL_ERROR);
    }
   
    while(!feof(filePointer)) {
        int loaded = fread(buffer, 1, DNS_PAYLOAD_LEN, filePointer);
        buffer[loaded] = 0;
        printf("-------------------\n%s\n-------------------\n", buffer);
    }

    if(filePointer != stdin)
        fclose(filePointer);
}

char *encodeData(char* data){
    return data;
}


