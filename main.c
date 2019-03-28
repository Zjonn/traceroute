#include "netinet/ip.h"
#include "netinet/ip_icmp.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

#include "inttypes.h"
#include "stdio.h"

#include "defines.h"
#include "recive.h"
#include "send.h"
#include "utils.h"

static int check_input(int argc, char* argv[]);
static void print_results(int ttl, reply_data (*replies)[PACKEGES_PER_TTL]);

int main(int argc, char* argv[])
{
    if(!check_input(argc, argv))
    {
        perror("Incorrect ip adress format");
        return EXIT_FAILURE;
    }

    LOG("MY PID %d", getppid());

    int socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(socketfd < 0)
    {
        EXIT_ERR("socket error");
    }

    reply_data replies[PACKEGES_PER_TTL];
    for(int ttl = 1; ttl < MAX_TTL; ttl++)
    {
        memset(replies, 0, sizeof(replies));

        send_pings(socketfd, argv[1], ttl);

        LOG("");

        recive(socketfd, ttl, &replies);

        LOG("");
        print_results(ttl, &replies);
    }
    return 0;
}

static int check_input(int argc, char* argv[])
{
    if(argc < 2)
        perror("No arguments, ip adress required");
    else if(argc > 2)
        perror("To many arguments");
    else
    {
        struct sockaddr_in sa;
        int                res = inet_pton(AF_INET, argv[1], &(sa.sin_addr));
        return res != 0;
    }
    return 1;
}

static void print_results(int ttl, reply_data (*replies)[PACKEGES_PER_TTL])
{
    int8_t  is_reply = 0, no_avg = 0;
    int16_t avg = 0;
    for(int i = 0; i < PACKEGES_PER_TTL; i++)
    {
        is_reply += replies[i]->recived;
        no_avg += replies[i]->recived == 0;
        avg += replies[i]->us / 1000;
        LOG("%d ", replies[i]->recived);
    }
    avg /= PACKEGES_PER_TTL;

    printf("%d. ", ttl);
    if(!is_reply)
    {
        printf("*\n");
        return;
    }
    for(int i = 0; i < PACKEGES_PER_TTL; i++)
    {
        if(replies[i]->recived)
            printf("%s ", replies[i]->ip_addr);

        for(int j = PACKEGES_PER_TTL - 1; j > i; j--)
        {
            if(strcmp(replies[i]->ip_addr, replies[j]->ip_addr) == 0)
                replies[j]->recived = 0;
        }
    }

    if(no_avg)
        printf("%s\n", "???");
    else
        printf("%d\n", avg);
}
