/**
 * @file error.h
 * @author Kristián Kičinka (xkicin02)
 * @brief 
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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
