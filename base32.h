#ifndef BASE32_H
#define BASE32_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int base32_decode(const uint8_t *encoded, uint8_t *result, int bufSize);
int base32_encode(const uint8_t *data, int length, uint8_t *result, int bufSize);

#endif // BASE32_H

