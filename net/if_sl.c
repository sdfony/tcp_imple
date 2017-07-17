#include "if.h"
#include "..\sys\mbuf.h"
#include "..\net\bpf.h"
#include "..\net\slip.h"
#include "if_slvar.h"
#include "if_types.h"
#include <stddef.h>

#define NSL 32
/*
 * SLMAX is a hard limit on input packet size.  To simplify the code
 * and improve performance, we require that packets fit in an mbuf
 * cluster, and if we get a compressed packet, there's enough extra
 * room to expand the header into a max length tcp/ip header (128
 * bytes).  So, SLMAX can be at most
 *	MCLBYTES - 128
 *
 * SLMTU is a hard limit on output packet size.  To insure good
 * interactive response, SLMTU wants to be the smallest size that
 * amortizes the header cost.  (Remember that even with
 * type-of-service queuing, we have to wait for any in-progress
 * packet to finish.  I.e., we wait, on the average, 1/2 * mtu /
 * cps, where cps is the line speed in characters per second.
 * E.g., 533ms wait for a 1024 byte MTU on a 9600 baud line.  The
 * average compressed header size is 6-8 bytes so any MTU > 90
 * bytes will give us 90% of the line bandwidth.  A 100ms wait is
 * tolerable (500ms is not), so want an MTU around 296.  (Since TCP
 * will send 256 byte segments (to allow for 40 byte headers), the
 * typical packet size on the wire will be around 260 bytes).  In
 * 4.3tahoe+ systems, we can set an MTU in a route so we do that &
 * leave the interface MTU relatively high (so we don't IP fragment
 * when acting as a gateway to someone using a stupid MTU).
 *
 * Similar considerations apply to SLIP_HIWAT:  It's the amount of
 * data that will be queued 'downstream' of us (i.e., in clists
 * waiting to be picked up by the tty output interrupt).  If we
 * queue a lot of data downstream, it's immune to our t.o.s. queuing.
 * E.g., if SLIP_HIWAT is 1024, the interactive traffic in mixed
 * telnet/ftp will see a 1 sec wait, independent of the mtu (the
 * wait is dependent on the ftp window size but that's typically
 * 1k - 4k).  So, we want SLIP_HIWAT just big enough to amortize
 * the cost (in idle time on the wire) of the tty driver running
 * off the end of its clists & having to call back slstart for a
 * new packet.  For a tty interface with any buffering at all, this
 * cost will be zero.  Even with a totally brain dead interface (like
 * the one on a typical workstation), the cost will be <= 1 character
 * time.  So, setting SLIP_HIWAT to ~100 guarantees that we'll lose
 * at most 1% while maintaining good interactive response.
 */
#if NBPFILTER > 0
#define	BUFOFFSET	(128+sizeof(struct ifnet **)+SLIP_HDRLEN)
#else
#define	BUFOFFSET	(128+sizeof(struct ifnet **))
#endif
#define	SLMAX		(MCLBYTES - BUFOFFSET)
#define	SLBUFSIZE	(SLMAX + BUFOFFSET)
#define	SLMTU		296
#define	SLIP_HIWAT	roundup(50,CBSIZE)
#define	CLISTRESERVE	1024	/* Can't let clists get too low */

/*
 * SLIP ABORT ESCAPE MECHANISM:
 *	(inspired by HAYES modem escape arrangement)
 *	1sec escape 1sec escape 1sec escape { 1sec escape 1sec escape }
 *	within window time signals a "soft" exit from slip mode by remote end
 *	if the IFF_DEBUG flag is on.
 */
#define	ABT_ESC		'\033'	/* can't be t_intr - distant host must know it*/
#define	ABT_IDLE	1	/* in seconds - idle before an escape */
#define	ABT_COUNT	3	/* count of escapes for abort */
#define	ABT_WINDOW	(ABT_COUNT*2+2)	/* in seconds - time to count */

#define FRAME_END	 	0xc0		/* Frame End */
#define FRAME_ESCAPE		0xdb		/* Frame Esc */
#define TRANS_FRAME_END	 	0xdc		/* transposed frame end */
#define TRANS_FRAME_ESCAPE 	0xdd		/* transposed frame esc */

struct sl_softc sl_softc[NSL];

#define FRAME_END	 	0xc0		/* Frame End */
#define FRAME_ESCAPE		0xdb		/* Frame Esc */
#define TRANS_FRAME_END	 	0xdc		/* transposed frame end */
#define TRANS_FRAME_ESCAPE 	0xdd		/* transposed frame esc */

extern int ifqmaxlen;
void slattach()
{
    int i = 0;
	struct sl_softc *sc;

    for (sc = sl_softc; i < NSL; sc++)
    {
        sc->sc_if.if_name = "sl";
		sc->sc_if.if_next = NULL;
		sc->sc_if.if_unit = i++;
        sc->sc_if.if_flags = IFF_POINTOPOINT | IFF_MULTICAST | SC_AUTOCOMP;
        sc->sc_if.if_snd.ifq_maxlen = ifqmaxlen;

        sc->sc_if.if_mtu = SLMTU;
        sc->sc_if.if_type = IFT_SLIP;
        sc->sc_if.if_metric = 0;
        sc->sc_if.if_baudrate = 0;

        sc->sc_fastq.ifq_maxlen = 32;
        sc->sc_flags = SC_AUTOCOMP | SC_NOICMP;

        sc->sc_if.if_output = sloutput;
        sc->sc_if.if_ioctl = slioctl;

		if_attach(&sc->sc_if);
        bpfattach(sc->sc_bpf, &sc->sc_if, DLT_SLIP, SLIP_HDRLEN);
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

static struct mbuf *sl_btom(struct sl_softc *sc, int len)
{
	return NULL;
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

