#include "error.h"

/**
 * @brief Funkcia zabezpečuje spracovanie chýb programu
 * 
 * @param errorType Typ chyby
 */
void proccessError(ErrorType errorType){
    switch (errorType){
        case ARGUMENTS_ERROR:
            printf("Argument error !\n");
            break;
        case INTERNAL_ERROR:
            printf("Internal error !\n");
            break;
        case BASE_HOST_ERROR:
            printf("Base host error !\n");
            break;
        default:
            break;
    }
    exit(errorType);
}
