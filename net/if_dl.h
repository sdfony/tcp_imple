#ifndef IF_DL_ADDR_H
#define IF_DL_ADDR_H

#include "..\sys\types.h"

struct sockaddr_dl
{
    u_char sdl_len;
    u_char sdl_family;
    u_short sdl_index;
    u_char sdl_type; // same as if_data: ifi_type
    u_char sdl_nlen;
    u_char sdl_alen;
    u_char sdl_slen;
    char sdl_data[12];
};

#define LLADDR(s) ((caddr_t)((s)->sdl_data + (s)->sdl_nlen))

void	link_addr (const char *, struct sockaddr_dl *);
char	*link_ntoa (const struct sockaddr_dl *);

#endif  // IF_DL_ADDR_H
