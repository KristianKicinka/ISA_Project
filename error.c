#include "error.h"

void proccessError(ErrorType errorType){
    switch (errorType){
        case ARGUMENTS_ERROR:
            printf("Argument error !\n");
            break;
        case INTERNAL_ERROR:
            printf("Internal error !\n");
            break;
        default:
            break;
    }
    exit(errorType);
}
