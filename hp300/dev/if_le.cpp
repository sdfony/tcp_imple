#include "if_le.h"
#include "../mbuf/mbuf/mbuf.h"
#include <stdio.h>
#include <string.h>
#include "if_queue.h"

#define ETHERMTU 1500
#define senderr(e) {error = e; goto bad;}
extern u_char etherbroadcastaddr[6];

struct le_softc
{
    struct arpcom sc_ac;
#define sc_if sc_ac.ac_if
#define sc_addr sc_ac.ac_enaddr
    // device-specific members
};

struct le_softc le_softc[NLE];


struct hp_device
{
	int hp_unit;
	caddr_t hp_addr;
};

int leattach(struct hp_device *hd)
{
    struct le_softc &sc = le_softc[hd->hp_unit];
    struct ifnet &ifnet = sc.sc_if;

    memcpy(sc.sc_addr, hd->hp_addr, sizeof (u_char));
    ifnet.if_name = "le";
	ifnet.if_next = nullptr;
    ifnet.if_unit = hd->hp_unit;
    ifnet.if_flags = IFF_SIMPLEX | IFF_BROADCAST | IFF_MULTICAST;
    ifnet.if_snd.ifq_maxlen = ifqmaxlen;

    ifnet.if_type = IFT_ETHER;
    ifnet.if_addrlen = 6;
    ifnet.if_hdrlen = 14;
    ifnet.if_mtu = ETHERMTU;
    ifnet.if_metric = 0;

    ifnet.if_init = leinit;
    ifnet.if_start = lestart;
    ifnet.if_ioctl = leioctl;
    ifnet.if_reset = lereset;

    if (ifnet.if_type == IFT_ETHER)
        ifnet.if_output = ether_output;

    if_attach(&ifnet);

	return 0;
}

int leinit(int)
{
	return 0;
}

int lereset(int)
{
	return 0;
}

void leread(int unit, char *buf, int len)
{
}


int lestart(struct ifnet *ifp)
{
	return 0;
}

int leioctl(struct ifnet *ifp, int cmd, caddr_t data)
{
	return 0;
}

int ether_output(struct ifnet *ifp,
	struct mbuf *m0,
	struct sockaddr *dst,
	struct rtentry *rt0)
{
	return 0;
}

void ether_attach(struct ifnet *ifp)
{
}
