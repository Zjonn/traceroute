/*
/ Daniel Dubiel 291111
*/
#include "send.h"
#include <arpa/inet.h>

#include "netinet/ip.h"
#include "netinet/ip_icmp.h"
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

#include "assert.h"
#include "inttypes.h"
#include "stdio.h"

#include "defines.h"
#include "utils.h"

static u_int16_t compute_icmp_checksum(const void* buff, int length);

static inline void set_icmphdr(struct icmphdr* icmp_hdr, int ttl)
{
    icmp_hdr->type             = ICMP_ECHO;
    icmp_hdr->code             = 0;
    icmp_hdr->un.echo.id       = getpid();
    icmp_hdr->un.echo.sequence = ttl;
    icmp_hdr->checksum         = 0;
    icmp_hdr->checksum = compute_icmp_checksum((u_int16_t*)icmp_hdr, sizeof(*icmp_hdr));
}

void send_pings(int sockfd, char ip_adr[], int ttl)
{
    struct icmphdr icmp_header;
    set_icmphdr(&icmp_header, ttl);

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    if(inet_pton(AF_INET, ip_adr, &recipient.sin_addr) <= 0)
        EXIT_ERR("");

    if(setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) == -1){
        EXIT_ERR("setsockopt error");
    }

    LOG_BYTES("ICMP_ECHO to send", &icmp_header, sizeof(icmp_header))
    for(int i = 0; i < PACKES_PER_TTL; i++)
    {
        ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 0,
                                    (struct sockaddr*)&recipient, sizeof(recipient));

        LOG("Nr %d with ttl %d", i, ttl);


        if(bytes_sent == -1)
        {
            EXIT_ERR("Unable to send echo request!");
        }
    }
}

static u_int16_t compute_icmp_checksum(const void* buff, int length)
{
    u_int32_t        sum;
    const u_int16_t* ptr = buff;
    assert(length % 2 == 0);

    for(sum = 0; length > 0; length -= 2)
        sum += *ptr++;

    sum = (sum >> 16) + (sum & 0xffff);
    return (u_int16_t)(~(sum + (sum >> 16)));
}