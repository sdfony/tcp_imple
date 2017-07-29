#include "../sys/types.h"
#include "../netinet/if_ether.h"
#include "if.h"
#include "if_dl.h"
#include "if_types.h"
#include <stdlib.h>
#include <string.h>

extern struct ifqueue pkintrq;

u_char	etherbroadcastaddr[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
extern	struct ifnet loif;
#define senderr(e) { error = (e); goto bad;}

/*
 * Ethernet output routine.
 * Encapsulate a packet of type family for the local net.
 * Use trailer local net encapsulation if enough data in first
 * packet leaves a multiple of 512 bytes of data in remainder.
 * Assumes that ifp is actually pointer to arpcom structure.
 */
int
ether_output(ifp, m0, dst, rt0)
	register struct ifnet *ifp;
	struct mbuf *m0;
	struct sockaddr *dst;
	struct rtentry *rt0;
{
    if (!(ifp->if_flags & IFF_UP))
        return;

    struct sockaddr_in *sin = (struct sockaddr_in *)dst;
    switch (sin->sin_family)
    {
    case AF_INET:
 //       if (!arpresolve())
            return 0;
        break;
    case AF_ISO:
        break;
    case AF_UNSPEC:
        break;
    default:
        break;
    }

    return 0;
}

/*
 * Process a received Ethernet packet;
 * the packet is in the mbuf chain m without
 * the ether header, which is provided separately.
 */
void
ether_input(ifp, eh, m)
	struct ifnet *ifp;
	register struct ether_header *eh;
	struct mbuf *m;
{
    struct timeval time;
    struct ifqueue *ifq;
    extern struct ifqueue ipintrq;

    ifp->if_lastchange = time;
    if (eh->ether_type == AF_INET)
        ifq = &ipintrq;
    else if (eh->ether_type == AF_ARP)
    {
        ifq = &arpintrq;
    }

    if (IF_QFULL(ifq))
    {
        IF_DROP(ifq);
    }
    else
    {
        IF_ENQUEUE(ifq, m);
    }
}

/*
 * Convert Ethernet address to printable (loggable) representation.
 */
static char digits[] = "0123456789abcdef";
char *
ether_sprintf(ap)
	register u_char *ap;
{
    return NULL;
}

/*
 * Perform common duties while attaching to interface list
 */
void
ether_ifattach(struct ifnet *ifp)
{
    struct ifaddr *addrlist = ifp->if_addrlist;

    ifp->if_type = IFT_ETHER;
    ifp->if_addrlen = 6;
    ifp->if_hdrlen = 14;
    ifp->if_mtu = ETHERMTU;

    for (; addrlist; addrlist = addrlist->ifa_next)
    {
        struct sockaddr_dl *dl = (struct sockaddr_dl *)(addrlist->ifa_addr);
        dl->sdl_alen = ifp->if_addrlen;

        memcpy(LLADDR(dl), ((struct arpcom*)ifp)->ac_enaddr, sizeof(((struct arpcom*)ifp)->ac_enaddr));
    }
}

u_char	ether_ipmulticast_min[6] = { 0x01, 0x00, 0x5e, 0x00, 0x00, 0x00 };
u_char	ether_ipmulticast_max[6] = { 0x01, 0x00, 0x5e, 0x7f, 0xff, 0xff };
/*
 * Add an Ethernet multicast address or range of addresses to the list for a
 * given interface.
 */
int
ether_addmulti(ifr, ac)
	struct ifreq *ifr;
	register struct arpcom *ac;
{
    return 0;
}

/*
 * Delete a multicast address record.
 */
int
ether_delmulti(ifr, ac)
	struct ifreq *ifr;
	register struct arpcom *ac;
{
    return 0;
}
