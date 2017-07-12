#ifndef IF_SLVAL_H
#define IF_SLVAL_H

#include "if.h"

#define SLIPDISC 0
#define NSL 32

#define MCLBYTES 2048
#define SLBUFSIZE 2048
#define SLIP_HDRLEN 16
#define BUFOFFSET 148
#define SLMAX 1900
#define SLMTU 296
#define SLIP_HIWAT 100

struct slcompress {};
struct dev_t {};

//  sc_if: if_flags, sc_flags
#define SC_COMPRESS 0
#define SC_NOICMP   1
#define SC_AUTOCOMP 2
#define SC_ERROR    3

/*
 * Definitions for SLIP interface data structures
 * 
 * (This exists so programs like slstats can get at the definition
 *  of sl_softc.)
 */
struct sl_softc {
	struct	ifnet sc_if;		/* network-visible interface */
	struct	ifqueue sc_fastq;	/* interactive output queue */
	struct	tty *sc_ttyp;		/* pointer to tty structure */
	u_char	*sc_mp;			/* pointer to next available buf char */
	u_char	*sc_ep;			/* pointer to last available buf char */
	u_char	*sc_buf;		/* input buffer */
	u_int	sc_flags;		/* see below */
	u_int	sc_escape;	/* =1 if last char input was FRAME_ESCAPE */
	long	sc_lasttime;		/* last time a char arrived */
	long	sc_abortcount;		/* number of abort esacpe chars */
	long	sc_starttime;		/* time of first abort in window */
#ifdef INET				/* XXX */
	struct	slcompress sc_comp;	/* tcp compression data */
#endif
	caddr_t	sc_bpf;			/* BPF data */
};

/* internal flags */
#define	SC_ERROR	0x0001		/* had an input error */

/* visible flags */
#define	SC_COMPRESS	IFF_LINK0	/* compress TCP traffic */
#define	SC_NOICMP	IFF_LINK1	/* supress ICMP traffic */
#define	SC_AUTOCOMP	IFF_LINK2	/* auto-enable TCP compression */

void slattach();
int slopen(dev_t dev, struct tty *tp);
void slinput(int c, struct tty *tp);
int sloutput(struct ifnet *ifp,
        struct mbuf *m,
        struct sockaddr *dst,
        struct rtentry *rtp);
void slstart(struct tty *tp);
void slclose(struct tty *tp);
int sltioctl(struct tty *tp, int cmd, caddr_t data, int flag);
int slioctl(struct ifnet *, int, caddr_t);

#endif  // IF_SLVAL_H

