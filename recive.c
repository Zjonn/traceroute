#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "netinet/ip.h"
#include "netinet/ip_icmp.h"
#include "recive.h"

int recive(int sockfd, int ttl, reply_data (*replies)[PACKEGES_PER_TTL])
{
    struct sockaddr_in sender;
    socklen_t          sender_len = sizeof(sender);
    u_int8_t           buffer[IP_MAXPACKET];
    // struct timeval     timeout __attribute__((unused));

    // timeout.tv_sec  = 1;
    // timeout.tv_usec = 0;

    for(int i = 0; i < PACKEGES_PER_TTL;)
    {
        ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0,
                                      (struct sockaddr*)&sender, &sender_len);
        LOG("%d ", replies[i]->recived);
        if(packet_len < 0)
        {
            EXIT_ERR("recvfrom error");
        }

        char tmp_sender_ip[20];
        inet_ntop(AF_INET, &(sender.sin_addr), tmp_sender_ip, sizeof(tmp_sender_ip));

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

        LOG("Is my icmp? %s\n",
            is_my_icmp(potential_reply, ttl) ? "Yes" : "No");

        if(is_my_icmp(potential_reply, ttl))
        {
            // replies[i]->recived = 1;
            LOG("%p %.2x", replies[i]->ip_addr, replies[i]->ip_addr[0]);
            memcpy(replies[i]->ip_addr, tmp_sender_ip, sizeof(tmp_sender_ip));
            LOG_BYTES("", replies[i]->ip_addr, sizeof(tmp_sender_ip));
            i++;
        }
        LOG_BYTES("", replies, sizeof(*replies));
    }

    return EXIT_SUCCESS;
}