#include "if.h"

#define LOMTU 1536

struct ifnet loif;

int loioctl(struct ifnet *, int, caddr_t);

void loopattach(int n)
{
    struct ifnet &ifnet = loif;

	ifnet.if_next = nullptr;
	ifnet.if_name = "lo";
    ifnet.if_flags = IFF_MULTICAST | IFF_LOOPBACK;
    ifnet.if_type = IFT_LOOP;
    ifnet.if_mtu = LOMTU;
    ifnet.if_metric = 0;

    ifnet.if_output = looutput;
    ifnet.if_ioctl = loioctl;

    if_attach(&ifnet);
}

int loioctl(struct ifnet *ifp, int cmd, caddr_t data)
{
	return 0;
}

int looutput(struct ifnet *ifp,
	struct mbuf *m,
	struct sockaddr *dst,
	struct rtentry *rtp)
{
	return 0;
}

/* ARGSUSED */
void
lortrequest(cmd, rt, sa)
	int cmd;
	struct rtentry *rt;
	struct sockaddr *sa;
{
}
