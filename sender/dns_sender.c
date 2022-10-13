#include "dns_sender.h"

int main(int argc, char const *argv[]){
    printf("Hello word ISA! from DNS Sender\n");

    SenderArguments *senderArguments = parseSenderArguments(argc, (char**) argv);

    printSenderArguments(senderArguments);

    clearSenderArguments(senderArguments);

    return 0;
}
