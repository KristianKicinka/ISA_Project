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
    if ((sock = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP)) == -1){
        perror("sock");
        proccessError(BASE_HOST_ERROR);
    }
        
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
    
    int bytes_to_write = strlen(payload);
    int segment_count = 0;
    bool last_label = false;
    unsigned char *current_query_position = query;

    while (!last_label){
        if (bytes_to_write > 63){
            *current_query_position++ = 63;
            for (int i = 0; i < 63; i++){
                *current_query_position++ = payload[segment_count * 63 + i];
            }
            bytes_to_write -= 63;
            segment_count++;
        }else{
            last_label = true;
            *current_query_position++ = bytes_to_write;
            for (int i = 0; i < bytes_to_write; i++){
                *current_query_position++ = payload[segment_count * 63 + i];
            }   
        } 
    }

    char base_part[strlen(base_host) + 2];
    translateToDNSquery(base_part, base_host);

    strcat((char*) query,base_part);
    current_query_position += strlen(base_part) + 1;

    *current_query_position++ = (unsigned char) 0;
    *current_query_position++ = (unsigned char) 0;
    *current_query_position++ = (unsigned char) 1;
    *current_query_position++ = (unsigned char) 0;
    *current_query_position++ = (unsigned char) 1;

    return current_query_position - query;
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
    query[lock + position + 1] = (unsigned char) 0;
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
        if (errno == EWOULDBLOCK){
            if(sendto(sock,(char*) dns_buffer,sizeof(DnsHeader) + query_length, 0, (struct sockaddr*)&destination, sizeof(destination)) < 0){
                proccessError(INTERNAL_ERROR);
            }
        }else if (result == -1){
            proccessError(INTERNAL_ERROR);
        }else if (result >= 0){
            break;
        }
    }

    close(sock);
    (void) recv_buffer;
}

/**
 * @brief Funkcia zabezpečuje získanie veľkosti zadaného súboru
 * 
 * @param file_path Cesta k súboru
 * @return int Veľkosť súboru
 */
int getFileSize(char *file_path){
    FILE *file = fopen(file_path, "r");

    if (file == NULL)
        proccessError(INTERNAL_ERROR);
    
    fseek(file, 0L, SEEK_END);
  
    int res = ftell(file);
  
    fclose(file);
    return res;  
}
