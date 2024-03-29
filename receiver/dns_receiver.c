/**
 * @file dns_receiver.c
 * @author Kristián Kičinka (xkicin02)
 * @brief 
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "dns_receiver.h"

#define FILE_PATH_LEN 1024
#define BASE_32_CHARSET "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"

char file_path[FILE_PATH_LEN] = {0};

int chunk_id = 0;

int glob_socket = 0;

int main(int argc, char const *argv[]){

    printf("----------------------------------------------------\n");
    printf("############### DNS Receiver started ###############\n");
    printf("----------------------------------------------------\n");

    ReceiverArguments *receiverArguments = parseReceiverArguments(argc, (char**) argv);

    printReceiverArguments(receiverArguments);

    int result;
    int sock = createSocket();
    
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        proccessError(INTERNAL_ERROR);
    
    glob_socket = sock;
    signal(SIGINT, closeSockets);

    socklen_t length;
    char recv_buffer[DNS_PACKET_LEN] = {0};
    PacketType packet_type;
    
    struct sockaddr_in receiver, sender_address;
    memset(&receiver, 0, sizeof(receiver));

    receiver.sin_family = AF_INET;     
    receiver.sin_addr.s_addr = htonl(INADDR_ANY);              
    receiver.sin_port =  htons(DNS_PORT);
    
    if (bind(sock, (struct sockaddr *)&receiver, sizeof(receiver)) == -1)
        proccessError(INTERNAL_ERROR);

    length = sizeof(sender_address);

    while ((result = recvfrom(sock, recv_buffer, DNS_PACKET_LEN, 0, (struct sockaddr *)&sender_address, &length )) >= 0){
        if(result == -1)   
            proccessError(INTERNAL_ERROR);

        DnsHeader *dns_header = (DnsHeader *) recv_buffer;
        char *data_payload = (char*) (recv_buffer + sizeof(DnsHeader));

        if (dns_header->rcode == INIT_PACKET_CODE){
            packet_type = INIT_PACKET;
            // Volanie dns_reciever_events funkcie
            dns_receiver__on_transfer_init(&sender_address.sin_addr);
        }else if (dns_header->rcode == DATA_PACKET_CODE){
            packet_type = DATA_PACKET;
        }else if (dns_header->rcode == END_PACKET_CODE){
            packet_type = END_PACKET;
            // Volanie dns_reciever_events funkcie
            dns_receiver__on_transfer_completed(file_path, getFileSize(file_path));
        }
    
        proccessDataPayload(data_payload, receiverArguments, &sender_address.sin_addr, packet_type);

        // send response
        sendConfirmPacket(sock, sender_address, recv_buffer);
    }

    clearReceiverArguments(receiverArguments);

    return 0;
}

/**
 * @brief Funkcia zabezpečuje spracovanie dátovej časti DNS paketu
 * 
 * @param data_payload Buffer obsahujúci dáta
 * @param arguments Argumenty skriptu dns_reciever
 * @param address Štruktúra obsahujúca adresu prijímateľa
 * @param type Typ spracovávaného paketu
 */
void proccessDataPayload(char *data_payload, ReceiverArguments *arguments, struct in_addr *address, PacketType type){
    unsigned char decoded_data[DNS_PACKET_LEN] = {0};
    char valid_base_host[DNS_PACKET_LEN] = {0};

    int data_length = strlen(data_payload) - strlen(arguments->BASE_HOST) - 1;
    char *recieved_base_host = (char *)(data_payload + data_length);

    int decoded_data_size = getDataFromPayload(data_payload, decoded_data, data_length, arguments->BASE_HOST, type);

    translateToDNSquery(valid_base_host, arguments->BASE_HOST);

    if (!strcmp(recieved_base_host, valid_base_host)){
        if (type == INIT_PACKET){
            strcpy(file_path,"");
            strcat(file_path, arguments->DST_FILEPATH);
            strcat(file_path, "/");
            strcat(file_path, (char *) decoded_data);
            remove(file_path);
        }else if (type == DATA_PACKET){
            // Volanie dns_reciever_events funkcie
            dns_receiver__on_chunk_received(address, file_path, chunk_id, decoded_data_size); 
            writeToFile(file_path, (char*) decoded_data, decoded_data_size);
            chunk_id++;
        }else if (type == END_PACKET){
            chunk_id = 0;
        }
            
    }
}

/**
 * @brief Funkcia zabezpečuje zápis získaných dát do súboru
 * 
 * @param path Cesta k súboru
 * @param data Dáta, ktoré majú byť zapísané
 * @param data_size Veľkosť dát
 */
void writeToFile(char *path, char *data, int data_size){
    FILE *file = fopen(path, "ab");
    if (file == NULL)
        proccessError(INTERNAL_ERROR);

    fwrite(data, 1, data_size, file);
    fclose(file);
    printf("[INFO] Data was written to file !!\n");
}

/**
 * @brief Funkcia zabezpečuje odoslanie odpovedi senderu
 * 
 * @param socket Číslo otvoreného soketu
 * @param destination Štruktúra s údajmi o prijímateľovi
 * @param recv_packet Paket, ktorý je nutné potvrdiť
 */
void sendConfirmPacket(int socket, struct sockaddr_in destination, char *recv_packet){
    unsigned char buffer[DNS_PACKET_LEN] = {0};
    memcpy(buffer, recv_packet, strlen(recv_packet) + 1);

    DnsHeader *dns_header = (DnsHeader *) buffer;

    dns_header->qr = 1;

    if(sendto(socket,(char*) buffer, sizeof(DnsHeader), 0, (struct sockaddr*)&destination, sizeof(destination)) < 0){
        proccessError(INTERNAL_ERROR);
    }

}

/**
 * @brief Funkcia transformuje dáta do dekodovaťeľnej formy a následne ich dekóduje
 * 
 * @param data_payload Buffer obsahujúci dáta
 * @param data Pole do ktorého sa uložia spracované dáta
 * @param data_size Veľkosť data_payloadu
 * @param base_host Base host zadaný používateľom
 * @param type Typ paketu
 */
int getDataFromPayload(char *data_payload, unsigned char *data, int data_size, char *base_host, PacketType type){
    unsigned char buffer[DNS_PACKET_LEN] = {0};
    int buffer_cnt = 0;

    for (int i = 0; i < data_size; i++){
        
        if(strchr(BASE_32_CHARSET, data_payload[i]) != NULL){
            buffer[buffer_cnt] = data_payload[i];
            buffer_cnt++;
        }
    }

    if (type == DATA_PACKET){
        // Volanie dns_reciever_events funkcie
        callParsedQuery((char*) buffer, base_host);
    }

    return base32_decode((u_int8_t*) buffer,(u_int8_t*) data, ENCODE_PAYLOAD_LEN);   
}

/**
 * @brief Funkcia zabezpečuje obsluhu k volaniu funkcie zo zadaného rozhrania
 * 
 * @param data_part Dátová časť obsahujúca zašifrované dáta
 * @param base_host Base host zadaný používateľom
 */
void callParsedQuery(char *data_part, char* base_host){
    char buffer[DNS_PACKET_LEN * 2] = {0};
    strcat(buffer, data_part);
    strcat(buffer, ".");
    strcat(buffer, base_host);

    // Volanie dns_reciever_events funkcie
    dns_receiver__on_query_parsed(file_path, buffer);
}

/**
 * @brief Funkcia zabezpečuje ukončenie otvorených soketov
 * 
 * @param sig číslo signálu
 */
void closeSockets(int sig){
    close(glob_socket);
    exit(0);
}
