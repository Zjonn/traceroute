/*
/ Daniel Dubiel 291111
*/
#include "utils.h"

void print_as_bytes(unsigned char* buff, ssize_t length)
{
    for(ssize_t i = 0; i < length; i++, buff++)
        printf("%.2x ", *buff);
}