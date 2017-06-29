#ifndef SOCKET_H
#define SOCKET_H

#include "../types.h"

// sockaddr: sa_family
// domain: dom_family
enum { AF_INET, AF_ISO, AF_UNIX, AF_ROUTE, AF_LINK, AF_UNSPEC };

struct sockaddr
{
    u_char sa_len;
    u_char sa_family;
    char sa_data[14];
};

struct osockaddr
{
    u_short sa_family;
    char sa_data[14];
};

#endif  // SOCKET_H
