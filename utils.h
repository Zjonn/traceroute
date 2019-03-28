#pragma once

#include "netinet/ip.h"
#include "netinet/ip_icmp.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>

#ifdef DEBUG
#define LOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#define LOG_BYTES(msg, buff, length)  \
    {                                 \
        printf(msg " ");              \
        print_as_bytes((unsigned char *)(buff), length); \
        printf("\n");                 \
    }
#else
#define LOG(format, ...)
#define LOG_BYTES(msg, buff, length)
#endif

#define EXIT_ERR(MSG)                                    \
    fprintf(stderr, MSG " ERRNO %s\n", strerror(errno)); \
    exit(EXIT_FAILURE)

typedef struct
{
    int32_t  recived;
    char    ip_addr[20];
    int32_t ms;
} reply_data;

void print_as_bytes(unsigned char* buff, ssize_t length);

static inline struct icmphdr* get_icmphdr(struct iphdr* ip_hdr)
{
    return (struct icmphdr*)((void*)ip_hdr + 4 * ip_hdr->ihl);
}

static inline int is_my_icmp(struct icmphdr* icmp_hdr, int ttl)
{
    return icmp_hdr->un.echo.id == getpid() && icmp_hdr->un.echo.sequence == ttl;
}
