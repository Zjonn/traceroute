#include "send.h"

#include <arpa/inet.h>
#include <linux/icmp.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

#include "inttypes.h"
#include "stdio.h"
#include "assert.h"

#include "utils.h"

static u_int16_t compute_icmp_checksum(const void *buff, int length);

void send_pings(int sockfd, char ip_adr[], int ttl, int n)
{
  struct icmphdr icmp_header;

  icmp_header.type = ICMP_ECHO;
  icmp_header.code = 0;
  icmp_header.un.echo.id = getpid();

  struct sockaddr_in recipient;
  bzero(&recipient, sizeof(recipient));
  recipient.sin_family = AF_INET;
  inet_pton(AF_INET, ip_adr, &recipient.sin_addr);

  setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

  for (int i = 0; i < n; i++)
  {
    icmp_header.un.echo.sequence = ttl << 8 | i;
    icmp_header.checksum = 0;
    icmp_header.checksum =
        compute_icmp_checksum((u_int16_t *)&icmp_header, sizeof(icmp_header));

    ssize_t bytes_sent =
        sendto(sockfd, &icmp_header, sizeof(icmp_header), 0,
               (struct sockaddr *)&recipient, sizeof(recipient));

    log("ECHO_REQ nr %i send with ttl %d", i, ttl);

    if (bytes_sent == -1)
    {
      fprintf(stderr, "Unable to send echo request! Errno: %s", strerror(errno));
      exit(666);
    }
  }
}

static u_int16_t compute_icmp_checksum(const void *buff, int length)
{
  u_int32_t sum;
  const u_int16_t *ptr = buff;
  assert(length % 2 == 0);

  for (sum = 0; length > 0; length -= 2)
    sum += *ptr++;

  sum = (sum >> 16) + (sum & 0xffff);
  return (u_int16_t)(~(sum + (sum >> 16)));
}