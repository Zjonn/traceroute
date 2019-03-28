#pragma once

#include "utils.h"
#include "defines.h"

int receive(int sockfd, int ttl, reply_data (*replies)[PACKES_PER_TTL]);