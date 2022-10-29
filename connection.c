#include "connection.h"

#define PACKET_ID 1489
#define DNS_PORT 53
#define DNS_PACKET_LEN 512


void initSenderDNSheader(unsigned char *buffer){
    DnsHeader *dns_header = (DnsHeader *)buffer;

    dns_header->id = htons(PACKET_ID);
    dns_header->rd = 1;
    dns_header->qdcount = htons(1);
}

void initReceiverDNSheader(unsigned char *buffer){
     DnsHeader *dns_header = (DnsHeader *)buffer;

    dns_header->id = htons(PACKET_ID);
    dns_header->rd = 1;
    dns_header->qdcount = htons(1);
}

int createSocket(){
    int sock;
    if ((sock = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP)) == -1)
        proccessError(INTERNAL_ERROR);
    return sock;
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

void sendDataToDnsIP(char *ip_address, char *base_host, char *dataPayload){

    unsigned char dns_buffer[DNS_PACKET_LEN] = {0};
    unsigned char recv_buffer[DNS_PACKET_LEN] = {0};

    struct sockaddr_in destination;

    int sock = createSocket();

    destination.sin_family = AF_INET;
	destination.sin_port = htons(DNS_PORT);
	destination.sin_addr.s_addr = inet_addr(ip_address);

    initSenderDNSheader(dns_buffer);
    unsigned char *dns_query = dns_buffer + sizeof(DnsHeader);
    
    int query_length = createDNSquery(dns_query, dataPayload, base_host);

    if(sendto(sock,(char*)dns_buffer,sizeof(DnsHeader) + query_length, 0, (struct sockaddr*)&destination, sizeof(destination)) < 0){
        proccessError(INTERNAL_ERROR);
    }

    int destination_size = sizeof(destination);
    if (recvfrom(sock, (char*)recv_buffer, sizeof(recv_buffer), MSG_WAITALL, (struct sockaddr*) &destination, (socklen_t*)&destination_size) < 0){
        proccessError(INTERNAL_ERROR);
    }
    
    (void) recv_buffer;

}

