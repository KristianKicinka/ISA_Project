#include "dns_receiver.h"

int main(int argc, char const *argv[]){
    printf("Hello word ISA! from DNS Reciever\n");

    ReceiverArguments *receiverArguments = parseReceiverArguments(argc, (char**) argv);

    printReceiverArguments(receiverArguments);

    clearReceiverArguments(receiverArguments);

    return 0;
}

