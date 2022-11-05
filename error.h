#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    INTERNAL_ERROR  = 1,
    ARGUMENTS_ERROR = 2,
    BASE_HOST_ERROR = 3,
}ErrorType;

void proccessError(ErrorType errorType);

#endif // ERROR_H
