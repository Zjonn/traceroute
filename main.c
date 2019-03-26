#include <arpa/inet.h>
#include <linux/icmp.h>
#include <unistd.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>

#include "inttypes.h"
#include "stdio.h"

#include "utils.h"
#include "recive.h"
#include "send.h"

static int check_input(int argc, char *argv[]);

#define MAX_TTL 30

int main(int argc, char *argv[])
{
    if (!check_input(argc, argv))
    {
        perror("Incorrect ip adress format");
        return EXIT_FAILURE;
    }

    int socketfd __attribute__((unused)) = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socketfd < 0)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    for (int i = 1; i < MAX_TTL; i++)
    {
    }
    return 0;
}

static int check_input(int argc, char *argv[])
{
    if (argc < 2)
        perror("No arguments, ip adress required");
    else if (argc > 2)
        perror("To many arguments");
    else
    {
        struct sockaddr_in sa;
        int res = inet_pton(AF_INET, argv[1], &(sa.sin_addr));
        return res != 0;
    }
    return 1;
}
