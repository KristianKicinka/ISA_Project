#include "dns_sender.h"

#define PORT 8080
#define DNS_PAYLOAD_LEN 128
#define DNS_PORT 53 

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
    //loadData(senderArguments);

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

    dataPayload = encodeData(dataPayload);
    
    if(senderArguments->UPSTREAM_DNS_IP != NULL)
        sendDataToDnsIP(senderArguments, dataPayload);
    else{
        if ((hostent = gethostbyname(dataPayload)) == NULL)
            proccessError(INTERNAL_ERROR);
    }
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
        sendDnsData(senderArguments, buffer);
    }

    if(filePointer != stdin)
        fclose(filePointer);
}

char *encodeData(char* data){
    return data;
}

void sendDataToDnsIP(SenderArguments *senderArguments, char *dataPayload){

    unsigned char dns_buffer[512] = {0};
    struct sockaddr_in destination;

    int sock = createSocket();

    destination.sin_family = AF_INET;
	destination.sin_port = htons(DNS_PORT);
	destination.sin_addr.s_addr = inet_addr(senderArguments->UPSTREAM_DNS_IP);

    initDnsHeader(dns_buffer);
    unsigned char *dns_query = dns_buffer + sizeof(DnsHeader);
    
    int query_length = createDNSquery(dns_query, dataPayload, senderArguments->BASE_HOST);

    if(sendto(sock,(char*)dns_buffer,sizeof(DnsHeader) + query_length, 0, (struct sockaddr*)&destination, sizeof(destination)) < 0){
        proccessError(INTERNAL_ERROR);
    }

}

void initDnsHeader(unsigned char *buffer){
    DnsHeader *dns_header = (DnsHeader *)buffer;

    dns_header->id = htons(1489);
    dns_header->rd = 1;
    dns_header->qdcount = htons(1);

}
// Prelozenie formatu alebo kolko ostava
int createDNSquery(unsigned char *query, char *payload, char *base_host){
    int char_position = 0;
    int segment_count = 0;
    int total_length = 0;
    
    for (int i = 0; i < strlen(payload); i++){
        if (char_position == 63 || i == 0){
            int remaining_payload = strlen(payload) - (segment_count * 63);
            if (remaining_payload > 63)
                query[segment_count * 64 + char_position ] = (unsigned char) 63;
            else
                query[segment_count * 64 + char_position ] = (unsigned char) remaining_payload;

            char_position = 0;
            if (i != 0){
                segment_count++;
            }
        }
        
        query[segment_count * 64 + char_position + 1] = payload[i];
        char_position++;
    }

    char base_part[strlen(base_host) + 2];
    translateToDNSquery(base_part, base_host);

    strcat((char*)query,base_part);
    total_length = strlen(payload) + strlen(base_part) + segment_count;

    query[total_length + 1] = (unsigned char) 0;

    query[total_length + 2] = (unsigned char) 0;
    query[total_length + 3] = (unsigned char) 1;
    query[total_length + 4] = (unsigned char) 0;
    query[total_length + 5] = (unsigned char) 1;
    
    return total_length + 6;
}

void translateToDNSquery(char* query, char *data){
    int lock = 0;
    int position = 0;

    for (int i = 0; i < strlen(data); i++){
        if (data[i] == '.'){
            query[lock] = (unsigned char) position;
            lock = i+1;
            position = 0;
        }else{
            query[i+1] = data[i];
            position ++;
        }
    }
    query[lock] = (unsigned char) position;
}


