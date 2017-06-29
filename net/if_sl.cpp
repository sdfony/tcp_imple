#include "if_slvar.h"
#include "if_queue.h"
#include "..\mbuf\mbuf\mbuf.h"
#include <cstddef>

#ifdef sc_if
#undef sc_if
#endif  // sc_if

#define SLMTU 296

struct sl_softc sl_softc[NSL];

void slattach()
{
    int i = 0;
    for (auto &sl : sl_softc)
    {
        auto &ifnet = sl.sc_if;

        ifnet.if_name = "sl";
		ifnet.if_next = nullptr;
		ifnet.if_unit = i++;
        ifnet.if_flags = IFF_POINTOPOINT;
        ifnet.if_snd.ifq_maxlen = ifqmaxlen;

        ifnet.if_mtu = SLMTU;
        ifnet.if_type = IFT_SLIP;
        ifnet.if_metric = 0;
        ifnet.if_baudrate = 0;

        sl.sc_fastq.ifq_maxlen = 32;
        sl.sc_flags = SC_AUTOCOMP | SC_NOICMP;

        //ifnet.if_init = ;
        ifnet.if_output = sloutput;
        //ifnet.if_start = slstart;
        ifnet.if_ioctl = slioctl;
        //ifnet.if_reset = ;

		if_attach(&ifnet);
    }
}

int slopen(dev_t dev, struct tty *tp)
{
    return 0;
}

static int slinit(struct sl_softc *sc)
{

    return 1;
}

struct mbuf *sl_btom(struct sl_softc *sc, int len)
{
	return nullptr;
}

void slinput(int c, struct tty *tp)
{
}

int sloutput(struct ifnet *ifp,
        struct mbuf *m,
        struct sockaddr *dst,
        struct rtentry *rtp)
{
    return 0;
}

void slstart(struct tty *tp)
{

}

void slclose(struct tty *tp)
{
}

int sltioctl(struct tty *tp, int cmd, caddr_t data, int flag)
{
	return 0;
}

int slioctl(struct ifnet *ifnet, int cmd, caddr_t data)
{
	return 0;
}

