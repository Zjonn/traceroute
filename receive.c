/*
/ Daniel Dubiel 291111
*/
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "netinet/ip.h"
#include "netinet/ip_icmp.h"
#include "receive.h"

int receive(int sockfd, int ttl, reply_data (*replies)[PACKES_PER_TTL])
{
    int                is_reached = 0;
    struct sockaddr_in sender;
    socklen_t          sender_len = sizeof(sender);
    u_int8_t           buffer[IP_MAXPACKET];
    struct timeval     timeout;

    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;

    for(int i = 0; i < PACKES_PER_TTL;)
    {
        fd_set descriptors;
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);
        int ready = select(sockfd + 1, &descriptors, NULL, NULL, &timeout);

        if(ready < 0)
        {
            EXIT_ERR("select error");
        }
        if(ready == 0)
            break;

        for(int j = 0; j < ready && i < PACKES_PER_TTL; j++)
        {
            ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0,
                                          (struct sockaddr*)&sender, &sender_len);
            if(packet_len < 0)
            {
                EXIT_ERR("recvfrom error");
            }

            char tmp_sender_ip[20] = { 0 };
            if(inet_ntop(AF_INET, &(sender.sin_addr), tmp_sender_ip,
                         sizeof(tmp_sender_ip)) == 0)
                EXIT_ERR("Sender ip corrupted!");


            LOG("Received IP packet with ICMP content from: %s", tmp_sender_ip);

            struct iphdr* ip_header = (struct iphdr*)buffer;

            LOG_BYTES("IP header", buffer, 4 * ip_header->ihl);
            LOG_BYTES("IP data", buffer + 4 * ip_header->ihl,
                      packet_len - 4 * ip_header->ihl);

            struct icmphdr* potential_reply = get_icmphdr(ip_header);

            if(potential_reply->type == ICMP_TIME_EXCEEDED)
            {
                struct iphdr* tmp =
                (struct iphdr*)((void*)potential_reply + sizeof(potential_reply));

                potential_reply = get_icmphdr(tmp);
            }

            LOG("Is my icmp? %s\n", is_my_icmp(potential_reply, ttl) ? "Yes" : "No");

            if(is_my_icmp(potential_reply, ttl))
            {
                memcpy((*replies)[i].ip_addr, tmp_sender_ip, sizeof(tmp_sender_ip));
                (*replies)[i].ms      = 1000 - timeout.tv_usec / 1000;
                (*replies)[i].recived = 1;

                is_reached |= potential_reply->type == ICMP_ECHOREPLY;
                i++;
            }
        }
    }

    return is_reached;
}