/**
 * @file connection.c
 * @author Kristián Kičinka (xkicin02)
 * @brief 
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "connection.h"

#define PACKET_ID 1444
#define DNS_PORT 53
#define DNS_PACKET_LEN 512

#define INIT_PACKET_CODE 12
#define DATA_PACKET_CODE 13
#define END_PACKET_CODE 14 


/**
 * @brief Funkcia zabezpečuje vytvorenie a nastavenie hlavičky DNS paketu
 * 
 * @param buffer DNS paket
 * @param packet_id Identifikátor DNS paketu
 * @param type Typ DNS paketu
 */
void initSenderDNSheader(unsigned char *buffer, int packet_id, PacketType type){
    DnsHeader *dns_header = (DnsHeader *)buffer;

    dns_header->id = htons(packet_id);
    dns_header->rd = 1;
    dns_header->qdcount = htons(1);
    
    if (type == INIT_PACKET)
        dns_header->rcode = INIT_PACKET_CODE;
    else if (type == DATA_PACKET)
        dns_header->rcode = DATA_PACKET_CODE;
    else if (type == END_PACKET)
        dns_header->rcode = END_PACKET_CODE;
}

/**
 * @brief Funkcia zabezpečuje vytvorenie a nastavenie soketu
 * 
 * @return int Číslo soketu
 */
int createSocket(){
    int sock;
    if ((sock = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP)) == -1)
        proccessError(INTERNAL_ERROR);
    return sock;
}

/**
 * @brief Funkcia zabezpečuje vytvorenie DNS query
 * 
 * @param query Výsledná DNS query
 * @param payload Dáta ktoré majú byť spracované do formy DNS query
 * @param base_host Base host zadaný ako parameter
 * @return int Dĺžka DNS query
 */
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

/**
 * @brief Funkcia zabezpečuje transformáciu dát na DNS query
 * 
 *  Funkcia inšpirovaná zdrojom: 
 *      https://www.binarytides.com/dns-query-code-in-c-with-linux-sockets/
 * 
 * @param query Výsledná DNS query
 * @param data Dáta, ktoré majú byť transformované
 */
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

/**
 * @brief Funkcia zabezpečuje odoslanie DNS paketu
 * 
 * @param destination Štruktúra obsahujúca informácie o prijímateľovi
 * @param base_host Base host zadaný ako parameter
 * @param dataPayload Dátová časť paketu
 * @param packet_id Identifikátor paketu
 * @param type Typ paketu
 */
void sendDataToDnsIP(struct sockaddr_in destination, char *base_host, char *dataPayload, int packet_id, PacketType type){

    unsigned char dns_buffer[DNS_PACKET_LEN] = {0};
    unsigned char recv_buffer[DNS_PACKET_LEN] = {0};

    int sock = createSocket();
    int result;

    initSenderDNSheader(dns_buffer, packet_id, type);
    unsigned char *dns_query = dns_buffer + sizeof(DnsHeader);
    
    int query_length = createDNSquery(dns_query, dataPayload, base_host);

    if(sendto(sock,(char*) dns_buffer,sizeof(DnsHeader) + query_length, 0, (struct sockaddr*)&destination, sizeof(destination)) < 0){
        proccessError(INTERNAL_ERROR);
    }

    int destination_size = sizeof(destination);
    while ((result = recvfrom(sock, (char*)recv_buffer, sizeof(recv_buffer), MSG_WAITALL, (struct sockaddr*) &destination, (socklen_t*)&destination_size))){
        if (result == EWOULDBLOCK){
            if(sendto(sock,(char*) dns_buffer,sizeof(DnsHeader) + query_length, 0, (struct sockaddr*)&destination, sizeof(destination)) < 0){
                proccessError(INTERNAL_ERROR);
            }
        }else if (result == -1){
            proccessError(INTERNAL_ERROR);
        }else if (result >= 0){
            break;
        }
    }

    (void) recv_buffer;
}

int getFileSize(char *file_path){
    FILE *file = fopen(file_path, "r");

    if (file == NULL)
        proccessError(INTERNAL_ERROR);
    
    fseek(file, 0L, SEEK_END);
  
    int res = ftell(file);
  
    fclose(file);
    return res;  
}
