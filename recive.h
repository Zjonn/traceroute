#pragma once

#include "utils.h"
#include "defines.h"

int recive(int sockfd, int ttl, reply_data (*replies)[PACKEGES_PER_TTL]);