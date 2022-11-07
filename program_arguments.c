/**
 * @file program_arguments.c
 * @author Kristián Kičinka (xkicin02)
 * @brief 
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "program_arguments.h"

/**
 * @brief Funkcia zabezpečuje uloženie argumentov do štruktúry
 * 
 * @param argument Argument skriptu
 * @param value Hodnota argumentu
 */
void setArgument(char **argument, char *value){
    if (value == NULL)
        proccessError(INTERNAL_ERROR);
        
    if(*argument == NULL){
        *argument = (char*) calloc(strlen(value)+1, sizeof(char));
        if (*argument == NULL)
            proccessError(INTERNAL_ERROR);
    }

    if (strlen(*argument) < strlen(value)+1){
        char *tmpStr = (char*) realloc(*argument, sizeof(char) * strlen(value)+1);
        if (tmpStr == NULL)
            proccessError(INTERNAL_ERROR);
        *argument = tmpStr;
    }
    strcpy(*argument, value);
}

/**
 * @brief Funkcia zabezpečuje inicializáciu SenderArguments štruktúry
 * 
 * @return SenderArguments* Odkaz na štruktúru
 */
SenderArguments *initSenderArguments(){
    SenderArguments *senderArguments = calloc(1, sizeof(SenderArguments));

    if(senderArguments == NULL)
        proccessError(INTERNAL_ERROR);
  
    return senderArguments;
}

/**
 * @brief Funkcia zabezpečuje spracovanie argumentov programu
 * 
 * @param argc Počet argumentov
 * @param argv Pole argumentov
 * @return SenderArguments* Odkaz na štruktúru SenderArguments
 */
SenderArguments *parseSenderArguments(int argc, char *argv[]){
    SenderArguments *senderArguments = initSenderArguments();

    if (argc == 3){
        setArgument(&(senderArguments->BASE_HOST), argv[1]);
        setArgument(&(senderArguments->DST_FILEPATH), argv[2]);
    }else if (argc == 4 && strcmp(argv[1],"-u") != 0 ){
        setArgument(&(senderArguments->BASE_HOST), argv[1]);
        setArgument(&(senderArguments->DST_FILEPATH), argv[2]);
        setArgument(&(senderArguments->SRC_FILEPATH), argv[3]);
    }else if (argc == 5 && strcmp(argv[1],"-u") == 0){
        setArgument(&(senderArguments->UPSTREAM_DNS_IP), argv[2]);
        setArgument(&(senderArguments->BASE_HOST), argv[3]);
        setArgument(&(senderArguments->DST_FILEPATH), argv[4]);
    }else if (argc == 6 && strcmp(argv[1],"-u") == 0){
        setArgument(&(senderArguments->UPSTREAM_DNS_IP), argv[2]);
        setArgument(&(senderArguments->BASE_HOST), argv[3]);
        setArgument(&(senderArguments->DST_FILEPATH), argv[4]);
        setArgument(&(senderArguments->SRC_FILEPATH), argv[5]);
    }else{
        proccessError(ARGUMENTS_ERROR);
    }

    return senderArguments;
}

/**
 * @brief Funkcia zabezpečuje vymazanie štruktúry SenderArguments
 * 
 * @param arguments Štruktúra SenderArguments
 */
void clearSenderArguments(SenderArguments *arguments){
    if (arguments != NULL){
        if (arguments->UPSTREAM_DNS_IP != NULL)
            free(arguments->UPSTREAM_DNS_IP);
        if (arguments->BASE_HOST != NULL)
            free(arguments->BASE_HOST);
        if (arguments->DST_FILEPATH != NULL)
            free(arguments->DST_FILEPATH);
        if (arguments->SRC_FILEPATH != NULL)
            free(arguments->SRC_FILEPATH);
        free(arguments);
    }
    
}

/**
 * @brief Funkcia umožnuje výpis sender argumentov
 * 
 * @param senderArguments Štruktúra sender argumentov
 */
void printSenderArguments(SenderArguments *senderArguments){
    printf("UPSTREAM_DNS_IP : %s\n",senderArguments->UPSTREAM_DNS_IP);
    printf("BASE_HOST : %s\n",senderArguments->BASE_HOST);
    printf("DST_FILEPATH : %s\n",senderArguments->DST_FILEPATH);
    printf("SRC_FILEPATH : %s\n",senderArguments->SRC_FILEPATH);
    printf("----------------------------------------------------\n");
}

/**
 * @brief Funkcia zabezpečuje inicializáciu reciever argumentov
 * 
 * @return ReceiverArguments* Štruktúra reciever argumentov
 */
ReceiverArguments *initReceiverArguments(){
    ReceiverArguments *receiverArguments = calloc(1, sizeof(ReceiverArguments));

    if(receiverArguments == NULL)
        proccessError(INTERNAL_ERROR);
  
    return receiverArguments;
}

/**
 * @brief Funkcia zabezpečuje spracovanie reciever argumentov
 * 
 * @param argc Počet argumentov skriptu
 * @param argv Pole argumentov skriptu
 * @return ReceiverArguments* Štruktúra reciever argumentov
 */
ReceiverArguments *parseReceiverArguments(int argc, char *argv[]){
    ReceiverArguments *receiverArguments = initReceiverArguments();

    if (argc == 3){
        setArgument(&(receiverArguments->BASE_HOST), argv[1]);
        setArgument(&(receiverArguments->DST_FILEPATH), argv[2]);
    }else{
        proccessError(ARGUMENTS_ERROR);
    }

    return receiverArguments;
}

/**
 * @brief Funkcia zabezpečuje vyčistenie štruktúry reciever argumentov
 * 
 * @param arguments Štruktúra reciever argumentov
 */
void clearReceiverArguments(ReceiverArguments *arguments){
    if (arguments != NULL){
        if (arguments->BASE_HOST != NULL)
            free(arguments->BASE_HOST);
        if (arguments->DST_FILEPATH != NULL)
            free(arguments->DST_FILEPATH);
        free(arguments);
    }
}

/**
 * @brief Funkcia umožňuje výpis reciever argumentov
 * 
 * @param receiverArguments Štruktúra reciever argumentov
 */
void printReceiverArguments(ReceiverArguments *receiverArguments){
    printf("BASE_HOST : %s\n",receiverArguments->BASE_HOST);
    printf("DST_FILEPATH : %s\n",receiverArguments->DST_FILEPATH);
    printf("----------------------------------------------------\n");
}
