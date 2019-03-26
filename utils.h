#pragma once

#include <stdio.h>
#include <stdlib.h>


#ifdef DEBUG
#define LOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#define LOG_BYTES(msg, buff, length)                         \
    {                                                        \
        printf(msg);                                         \
        print_as_bytes(*buff, length); \
        printf("\n");                                        \
    }
#else
#define LOG(format, ...)
#define LOG_BYTES(msg, buff, length)
#endif

void print_as_bytes(unsigned char *buff, ssize_t length)
{
    for (ssize_t i = 0; i < length; i++, buff++)
        printf("%.2x ", *buff);
}
