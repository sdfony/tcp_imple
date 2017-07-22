#include "if.h"
#include "if_types.h"
#include <stddef.h>

#define LOMTU 1536

struct ifnet loif;

int loioctl(struct ifnet *ifp, int, caddr_t);

void loopattach(int n)
{
    struct ifnet *ifp = &loif;

	ifp->if_next = NULL;
	ifp->if_name = "lo";
    ifp->if_flags = IFF_MULTICAST | IFF_LOOPBACK;
    ifp->if_type = IFT_LOOP;
    ifp->if_mtu = LOMTU;
    ifp->if_metric = 0;

    ifp->if_output = looutput;
    ifp->if_ioctl = loioctl;

    if_attach(ifp);
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
