/**
 * @file dns_sender.c
 * @author Kristián Kičinka (xkicin02)
 * @brief 
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "dns_sender.h"

#define LINE_LEN 300
#define END_PACKET_DATA "END_PACKET" 

int packet_id = PACKET_ID;
int chunk_id = 0;
int file_size = 0;

int main(int argc, char const *argv[]){

    printf("----------------------------------------------------\n");
    printf("################ DNS Sender started ################\n"); 
    printf("----------------------------------------------------\n");
    
    SenderArguments *senderArguments = parseSenderArguments(argc, (char**) argv);

    struct sockaddr_in sender;
    memset(&sender, 0, sizeof(sender));

    // Odoslanie init paketu
    sendSenderData(senderArguments, senderArguments->DST_FILEPATH, strlen(senderArguments->DST_FILEPATH), INIT_PACKET);


    // Spracovanie data paketu
    loadData(senderArguments);

    // // Odoslanie END paketu
    sendSenderData(senderArguments, END_PACKET_DATA, strlen(END_PACKET_DATA), END_PACKET);

    // Volanie dns_sender_events funkcie
    dns_sender__on_transfer_completed(senderArguments->DST_FILEPATH, file_size);

    clearSenderArguments(senderArguments);

    return 0;
}

/**
 * @brief Funkcia zabezpečuje inicializáciu štruktúry SenderData
 * 
 * @return SenderData* Výsledná štruktúra
 */
SenderData *initSenderData(){
    SenderData *senderData = calloc(1, sizeof(SenderData));
    if (senderData == NULL){
        proccessError(INTERNAL_ERROR);
    }
    return senderData;
}

/**
 * @brief Funkcia zabezpečuje čistenie štruktúry
 * 
 * @param senderData Štruktúra SenderData
 */
void clearSenderData(SenderData *senderData){
    if (senderData != NULL){
        free(senderData);
    }    
}

/**
 * @brief Funkcia zabezpečuje zakódovanie dát a výber cieľovej IP DNS servera
 * 
 * @param senderArguments Argumenty dns_sender skriptu
 * @param dataPayload Dátová časť payloadu
 * @param payload_size Veľkosť payloadu
 * @param type Typ DNS paketu
 */
void sendSenderData(SenderArguments *senderArguments, char *dataPayload, int payload_size , PacketType type){
    char encoded_data[ENCODE_PAYLOAD_LEN] = {0};
    char chunk_data[ENCODE_PAYLOAD_LEN * 2] = {0};

    base32_encode((uint8_t*)dataPayload, payload_size, (u_int8_t*)encoded_data, ENCODE_PAYLOAD_LEN);

    strcat(chunk_data, encoded_data);
    strcat(chunk_data,".");
    strcat(chunk_data, senderArguments->BASE_HOST);

    if(senderArguments->UPSTREAM_DNS_IP != NULL){
        if (type == INIT_PACKET)
            sendInitPacket(senderArguments->UPSTREAM_DNS_IP, encoded_data, senderArguments->BASE_HOST);
        else if(type == DATA_PACKET){
            // Volanie dns_sender_events funkcie
            dns_sender__on_chunk_encoded(senderArguments->DST_FILEPATH, chunk_id, chunk_data);
            sendDataPacket(senderArguments->UPSTREAM_DNS_IP, encoded_data, senderArguments->BASE_HOST, senderArguments->DST_FILEPATH, payload_size);
        }else if (type == END_PACKET)
            sendEndPacket(senderArguments->UPSTREAM_DNS_IP, encoded_data, senderArguments->BASE_HOST);
    }else{
        char dns_ip[LINE_LEN] = {0};
        getImplicitDNSserverIP(dns_ip);

        if (type == INIT_PACKET)
            sendInitPacket(dns_ip, encoded_data, senderArguments->BASE_HOST);
        else if(type == DATA_PACKET){
            // Volanie dns_sender_events funkcie
            dns_sender__on_chunk_encoded(senderArguments->DST_FILEPATH, chunk_id, chunk_data);
            sendDataPacket(dns_ip, encoded_data, senderArguments->BASE_HOST, senderArguments->DST_FILEPATH, payload_size);
        }else if (type == END_PACKET)
            sendEndPacket(dns_ip, encoded_data, senderArguments->BASE_HOST);
    }

}

/**
 * @brief Funkcia zabezpečuje načítanie dát zo súboru
 * 
 * @param senderArguments Argumenty dns_sender skriptu
 */
void loadData(SenderArguments *senderArguments){

    char load_buffer[PAYLOAD_LEN] = {0};
    FILE* filePointer;

    if (senderArguments->SRC_FILEPATH == NULL)
        filePointer = stdin;
    else{
        filePointer = fopen(senderArguments->SRC_FILEPATH, "rb");
        if (filePointer == NULL)
            proccessError(INTERNAL_ERROR);
    }
   
    while(!feof(filePointer)) {
        int loaded = fread(load_buffer, 1, PAYLOAD_LEN - 1, filePointer);
        load_buffer[loaded] = 0;
        file_size += loaded; 
        sendSenderData(senderArguments, load_buffer, loaded, DATA_PACKET);
    }

    if(filePointer != stdin)
        fclose(filePointer);

}

/**
 * @brief Funkcia zabezpečuje získanie IP DNS serveru z resolv.conf súboru
 * 
 * Funkcia inšpirovaná zdrojom: 
 *      https://www.binarytides.com/dns-query-code-in-c-with-linux-sockets/
 * 
 * @param ip_address IP adresa implicitného DNS servera
 * 
 */
void getImplicitDNSserverIP(char *ip_address){
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
                parse_item[strlen(parse_item) - 1] = 0;
                strcpy(ip_address, parse_item);
                fclose(resolv_file);
                return;
            }
            
        }
    }
    fclose(resolv_file);
}

/**
 * @brief Funkcia zabezpečuje nastavenie a odoslanie DNS init paketu
 * 
 * @param ip_address IP adresa DNS serveru (prijímateľa)
 * @param data Dáta DNS paketu
 * @param base_host Base host zadaný ako parameter skriptu
 */
void sendInitPacket(char *ip_address, char *data, char *base_host){
    struct sockaddr_in destination;

    destination.sin_family = AF_INET;
	destination.sin_port = htons(DNS_PORT);
	destination.sin_addr.s_addr = inet_addr(ip_address);

    sendDataToDnsIP(destination, base_host, data, packet_id, INIT_PACKET);

    // Volanie dns_sender_events funkcie
    dns_sender__on_transfer_init(&destination.sin_addr);
    printf("[INFO] Init packet was sent !!\n");
    packet_id++;
}

/**
 * @brief Funkcia zabezpečuje nastavenie a odoslanie DNS data paketu
 * 
 * @param ip_address IP adresa DNS serveru (prijímateľa)
 * @param data Dáta DNS paketu
 * @param base_host Base host zadaný ako parameter skriptu
 * @param chunk_size Veľkosť dát zašifrovaných v pakete
 */
void sendDataPacket(char *ip_address, char *data, char *base_host, char *file_path, int chunk_size){
    struct sockaddr_in destination;

    destination.sin_family = AF_INET;
	destination.sin_port = htons(DNS_PORT);
	destination.sin_addr.s_addr = inet_addr(ip_address);

    sendDataToDnsIP(destination, base_host, data, packet_id, DATA_PACKET);

    // Volanie dns_sender_events funkcie TODO
    dns_sender__on_chunk_sent(&destination.sin_addr, file_path, chunk_id, chunk_size);
    printf("[INFO] Data packet was sent !!\n");
    chunk_id++;
    packet_id++;
}

/**
 * @brief Funkcia zabezpečuje nastavenie a odoslanie DNS end paketu
 * 
 * @param ip_address IP adresa DNS serveru (prijímateľa)
 * @param data Dáta DNS paketu
 * @param base_host Base host zadaný ako parameter skriptu
 */
void sendEndPacket(char *ip_address, char *data, char *base_host){
    struct sockaddr_in destination;

    destination.sin_family = AF_INET;
	destination.sin_port = htons(DNS_PORT);
	destination.sin_addr.s_addr = inet_addr(ip_address);

    sendDataToDnsIP(destination, base_host, data, packet_id, END_PACKET);

    printf("[INFO] End packet was sent !!\n");
    packet_id++;
}
