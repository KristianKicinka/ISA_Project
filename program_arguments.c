#include "program_arguments.h"

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

SenderArguments *initSenderArguments(){
    SenderArguments *senderArguments = calloc(1, sizeof(SenderArguments));

    if(senderArguments == NULL)
        proccessError(INTERNAL_ERROR);
  
    return senderArguments;
}

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

void printSenderArguments(SenderArguments *senderArguments){
    printf("UPSTREAM_DNS_IP : %s\n",senderArguments->UPSTREAM_DNS_IP);
    printf("BASE_HOST : %s\n",senderArguments->BASE_HOST);
    printf("DST_FILEPATH : %s\n",senderArguments->DST_FILEPATH);
    printf("SRC_FILEPATH : %s\n",senderArguments->SRC_FILEPATH);
}

ReceiverArguments *initReceiverArguments(){
    ReceiverArguments *receiverArguments = calloc(1, sizeof(ReceiverArguments));

    if(receiverArguments == NULL)
        proccessError(INTERNAL_ERROR);
  
    return receiverArguments;
}

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

void clearReceiverArguments(ReceiverArguments *arguments){
    if (arguments != NULL){
        if (arguments->BASE_HOST != NULL)
            free(arguments->BASE_HOST);
        if (arguments->DST_FILEPATH != NULL)
            free(arguments->DST_FILEPATH);
        free(arguments);
    }
}

void printReceiverArguments(ReceiverArguments *receiverArguments){
    printf("BASE_HOST : %s\n",receiverArguments->BASE_HOST);
    printf("DST_FILEPATH : %s\n",receiverArguments->DST_FILEPATH);
}

