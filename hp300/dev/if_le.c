#include <stdio.h>
#include <string.h>
#include "device.h"
#include "..\hp300\isr.h"
#include "..\..\sys\mbuf.h"
#include "..\..\net\if.h"
#include "..\..\net\if_types.h"
#include "..\..\netinet\if_ether.h"
#include "..\..\net\bpf.h"

#define NLE 32
extern ifqmaxlen;

/* offsets for:	   ID,   REGS,    MEM,  NVRAM */
int	lestd[] = { 0, 0x4000, 0x8000, 0xC008 };

int	leattach();
struct	driver ledriver = {
	leattach, "le",
};

struct	isr le_isr[NLE];
int	ledebug = 0;		/* console error messages */

int	leintr(), leinit(), leioctl(), lestart(), ether_output(), lereset();
struct	mbuf *m_devget();
extern	struct ifnet loif;

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * le_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 */
struct	le_softc {
	struct	arpcom sc_ac;	/* common Ethernet structures */
#define	sc_if	sc_ac.ac_if	/* network-visible interface */
#define	sc_addr	sc_ac.ac_enaddr	/* hardware Ethernet address */
	struct	lereg0 *sc_r0;	/* DIO registers */
	struct	lereg1 *sc_r1;	/* LANCE registers */
	struct	lereg2 *sc_r2;	/* dual-port RAM */
	int	sc_rmd;		/* predicted next rmd to process */
	int	sc_tmd;		/* next available tmd */
	int	sc_txcnt;	/* # of transmit buffers in use */
	/* stats */
	int	sc_runt;
	int	sc_jab;
	int	sc_merr;
	int	sc_babl;
	int	sc_cerr;
	int	sc_miss;
	int	sc_rown;
	int	sc_xown;
	int	sc_xown2;
	int	sc_uflo;
	int	sc_rxlen;
	int	sc_rxoff;
	int	sc_txoff;
	int	sc_busy;
	short	sc_iflags;
} le_softc[NLE];

/* access LANCE registers */
#define	LERDWR(cntl, src, dst) \
	do { \
		(dst) = (src); \
	} while (((cntl)->ler0_status & LE_ACK) == 0);

int leattach(struct hp_device *hd)
{
    struct le_softc *sc = &le_softc[hd->hp_unit];
    struct ifnet *ifnet = &sc->sc_if;

    memcpy(sc->sc_addr, hd->hp_addr, sizeof (sc->sc_addr));
    ifnet->if_name = "le";
	ifnet->if_next = NULL;
    ifnet->if_unit = hd->hp_unit;
    ifnet->if_flags = IFF_SIMPLEX | IFF_BROADCAST | IFF_MULTICAST;
    ifnet->if_snd.ifq_maxlen = ifqmaxlen;

    ifnet->if_type = IFT_ETHER;
    ifnet->if_addrlen = 6;
    ifnet->if_hdrlen = 14;
    ifnet->if_mtu = ETHERMTU;
    ifnet->if_metric = 0;

    ifnet->if_init = leinit;
    ifnet->if_start = lestart;
    ifnet->if_ioctl = leioctl;
    ifnet->if_reset = lereset;
    ifnet->if_output = ether_output;

    bpfattach(&ifnet->if_bpf, ifnet, DLT_EN10MB, sizeof(struct ether_header));
    if_attach(ifnet);

	return 0;
}

int leinit(int a)
{
	return 0;
}

int lereset(int a)
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

void ether_attach(struct ifnet *ifp)
{
}

/*
 * Setup the logical address filter
 */
void
lesetladrf(sc)
	register struct le_softc *sc;
{
}

int ledrinit(ler2, le)
	register struct lereg2 *ler2;
	register struct le_softc *le;
{

    return 0;
}

int leintr(unit)
	register int unit;
{
	return 0;
}

/*
 * Ethernet interface transmitter interrupt.
 * Start another output if more data to send.
 */
int lexint(unit)
	register int unit;
{
	return 0;
}

/*
 * Ethernet interface receiver interrupt.
 * If input error just drop packet.
 * Decapsulate packet based on type and pass to type specific
 * higher-level input routine.
 */
int lerint(unit)
	int unit;
{
	return 0;
}

/*
 * Routine to copy from mbuf chain to transmit
 * buffer in board local memory.
 */
int leput(lebuf, m)
	register char *lebuf;
	register struct mbuf *m;
{
	return 0; 
}

leerror(unit, stat)
	int unit;
	int stat;
{
}
