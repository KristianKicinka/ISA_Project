#include "dns_receiver.h"

//#define PACKET_ID 1489
#define DNS_PORT 53
#define DNS_PACKET_LEN 512
#define FILE_PATH_LEN 1024 
#define ENCODE_PAYLOAD_LEN 256
#define BASE_32_CHARSET "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"

#define INIT_PACKET_CODE 12
#define DATA_PACKET_CODE 13 

char file_path[FILE_PATH_LEN] = {0};

int main(int argc, char const *argv[]){
    printf("Hello word ISA! from DNS Reciever\n");

    ReceiverArguments *receiverArguments = parseReceiverArguments(argc, (char**) argv);

    printReceiverArguments(receiverArguments);

    int result;
    int sock = createSocket();
    
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        proccessError(INTERNAL_ERROR);

    socklen_t length;
    char recv_buffer[DNS_PACKET_LEN] = {0};
    PacketType packet_type;
    
    struct sockaddr_in receiver, sender_address;
    memset(&receiver, 0, sizeof(receiver));

    receiver.sin_family = AF_INET;     
    receiver.sin_addr.s_addr = htonl(INADDR_ANY);              
    receiver.sin_port =  htons(DNS_PORT);
    
    if (bind(sock, (struct sockaddr *)&receiver, sizeof(receiver)) == -1) // binding with the port
        proccessError(INTERNAL_ERROR);

    length = sizeof(sender_address);

    while ((result = recvfrom(sock, recv_buffer, DNS_PACKET_LEN, 0, (struct sockaddr *)&sender_address, &length )) >= 0){
        if(result == -1)   
            proccessError(INTERNAL_ERROR);

        DnsHeader *dns_header = (DnsHeader *) recv_buffer;
        char *data_payload = (char*) (recv_buffer + sizeof(DnsHeader));

        if (dns_header->rcode == INIT_PACKET_CODE)
            packet_type = INIT_PACKET;
        else if (dns_header->rcode == DATA_PACKET_CODE)
            packet_type = DATA_PACKET;
    
        proccessDataPayload(data_payload, receiverArguments, packet_type);

        // send response
        sendConfirmPacket(sock, sender_address, recv_buffer);
    }

    clearReceiverArguments(receiverArguments);

    return 0;
}

void proccessDataPayload(char *data_payload, ReceiverArguments *arguments, PacketType type){
    printf("Data payload : %s \n", data_payload);
    unsigned char decoded_data[DNS_PACKET_LEN] = {0};
    char valid_base_host[DNS_PACKET_LEN] = {0};

    int data_length = strlen(data_payload) - strlen(arguments->BASE_HOST) - 1;
    char *recieved_base_host = (char *)(data_payload + data_length);

    getDataFromPayload(data_payload, decoded_data, data_length);

    translateToDNSquery(valid_base_host, arguments->BASE_HOST);

    if (!strcmp(recieved_base_host, valid_base_host)){
        if (type == INIT_PACKET){
            strcat(file_path, arguments->DST_FILEPATH);
            strcat(file_path, "/");
            strcat(file_path, (char *) decoded_data);
        }else if (type == DATA_PACKET)
            writeToFile(file_path, (char*) decoded_data);
    }
}

void writeToFile(char *path, char *data){
    FILE *file = fopen(path, "w");
    if (file == NULL)
        proccessError(INTERNAL_ERROR);

    fprintf(file,"%s",data);
    fclose(file);
    printf("Data was saved!!\n");
}

void sendConfirmPacket(int socket, struct sockaddr_in destination, char *recv_packet){
    unsigned char buffer[DNS_PACKET_LEN] = {0};
    memcpy(buffer, recv_packet, strlen(recv_packet) + 1);

    DnsHeader *dns_header = (DnsHeader *) buffer;

    dns_header->qr = 1;

    if(sendto(socket,(char*) buffer, strlen((char*) buffer), 0, (struct sockaddr*)&destination, sizeof(destination)) < 0){
        proccessError(INTERNAL_ERROR);
    }

}

void getDataFromPayload(char *data_payload, unsigned char *data, int data_size){
    unsigned char buffer[DNS_PACKET_LEN] = {0};
    int buffer_cnt = 0;

    for (int i = 0; i < data_size; i++){
        
        if(strchr(BASE_32_CHARSET, data_payload[i]) != NULL){
            buffer[buffer_cnt] = data_payload[i];
            buffer_cnt++;
        }
    }

    base32_decode((u_int8_t*) buffer,(u_int8_t*) data, ENCODE_PAYLOAD_LEN);
    
}
