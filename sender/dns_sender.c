#include "dns_sender.h"

#define PORT 8080
#define PAYLOAD_LEN 128
#define ENCODE_PAYLOAD_LEN 256
#define LINE_LEN 300 


int main(int argc, char const *argv[]){
    printf("Hello word ISA! from DNS Sender\n");

    SenderArguments *senderArguments = parseSenderArguments(argc, (char**) argv);

    printSenderArguments(senderArguments);

    struct sockaddr_in sender;
    memset(&sender, 0, sizeof(sender));

    //char *ip = getImplicitDNSserverIP();
    //printf("DNS SERVER IP IS : %s\n",ip);

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
    if (senderData != NULL){
        free(senderData);
    }    
}

void sendSenderData(SenderArguments *senderArguments, char *dataPayload){

    char encoded_data[ENCODE_PAYLOAD_LEN] = {0};
    base32_encode((uint8_t*)dataPayload, strlen(dataPayload), (u_int8_t*)encoded_data, ENCODE_PAYLOAD_LEN);

    printf("Encoded string : %s\n",encoded_data);
    
    if(senderArguments->UPSTREAM_DNS_IP != NULL)
        sendDataToDnsIP(senderArguments->UPSTREAM_DNS_IP, senderArguments->BASE_HOST, encoded_data);
    else{
        char *dns_ip = getImplicitDNSserverIP();
        sendDataToDnsIP(dns_ip, senderArguments->BASE_HOST, encoded_data);
    }

}

void loadData(SenderArguments *senderArguments){

    char load_buffer[PAYLOAD_LEN];
    FILE* filePointer;

    if (senderArguments->SRC_FILEPATH == NULL)
        filePointer = stdin;
    else{
        filePointer = fopen(senderArguments->SRC_FILEPATH, "r");
        if (filePointer == NULL)
            proccessError(INTERNAL_ERROR);
    }
   
    while(!feof(filePointer)) {
        int loaded = fread(load_buffer, 1, PAYLOAD_LEN, filePointer);
        load_buffer[loaded] = 0;
        printf("-------------------\n%s\n-------------------\n", load_buffer);
        sendSenderData(senderArguments, load_buffer);
        
    }

    if(filePointer != stdin)
        fclose(filePointer);
}

char *getImplicitDNSserverIP(){
    FILE *resolv_file = fopen("/etc/resolv.conf", "r");
    char line[LINE_LEN] = {0};
    char line_start[11] = {0};
    char *parse_item;

    if (resolv_file == NULL)
        proccessError(INTERNAL_ERROR);
    
    while (fgets(line, LINE_LEN, resolv_file) != NULL)  {

        if (line[0] != '#'){
            strncpy(line_start, line, 10);
            line_start[10] = '\0';
            if (!strcmp(line_start, "nameserver")){
                parse_item = strtok(line," ");
                parse_item = strtok(NULL," ");
                parse_item[strlen(parse_item) - 1] = 0; // Delete /n from end
                return parse_item;
            }
            
        }
    }
    return NULL;
}

