#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "utils.h"

int recive(int sockfd, int ttl)
{
    for (;;)
    {

        struct sockaddr_in sender;
        socklen_t sender_len = sizeof(sender);
        u_int8_t buffer[IP_MAXPACKET];

        ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr *)&sender, &sender_len);
        if (packet_len < 0)
        {
            fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        char sender_ip_str[20];
        inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
        LOG("Received IP packet with ICMP content from: %s", sender_ip_str);

        struct iphdr *ip_header = (struct iphdr *)buffer;
        ssize_t ip_header_len = 4 * ip_header->ihl;

        LOG_BYTES("IP header", buffer, ip_header_len);
        LOG_BYTES("IP data", buffer + ip_header_len, packet_len - ip_header_len);
    }

    return EXIT_SUCCESS;
}