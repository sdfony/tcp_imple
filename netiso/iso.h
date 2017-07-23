#ifndef NET_ISO_H 
#define NET_ISO_H

#include "../netinet/in.h"
#include "../sys/cdefs.h"

/*
 *	Return true if this is a multicast address
 *	This assumes that the bit transmission is lsb first. This 
 *	assumption is valid for 802.3 but not 802.5. There is a
 *	kludge to get around this for 802.5 -- see if_lan.c
 *	where subnetwork header is setup.
 */
#define	IS_MULTICAST(snpa)\
	((snpa)[0] & 0x01)
	
/*
 * Protocols
 */
#define	ISOPROTO_TCP	6		/* IETF experiment */
#define	ISOPROTO_UDP	17		/* IETF experiment */
#define	ISOPROTO_TP0	25		/* connection oriented transport protocol */
#define	ISOPROTO_TP1	26		/* not implemented */
#define	ISOPROTO_TP2	27		/* not implemented */
#define	ISOPROTO_TP3	28		/* not implemented */
#define	ISOPROTO_TP4	29		/* connection oriented transport protocol */
#define	ISOPROTO_TP		ISOPROTO_TP4	 /* tp-4 with negotiation */
#define	ISOPROTO_CLTP	30		/* connectionless transport (not yet impl.) */
#define	ISOPROTO_CLNP	31		/* connectionless internetworking protocol */
#define	ISOPROTO_X25	32		/* cons */
#define	ISOPROTO_INACT_NL	33	/* inactive network layer! */
#define	ISOPROTO_ESIS	34		/* ES-IS protocol */
#define	ISOPROTO_INTRAISIS	35		/* IS-IS protocol */
#define	ISOPROTO_IDRP	36		/* Interdomain Routing Protocol */

#define	ISOPROTO_RAW	255		/* raw clnp */
#define	ISOPROTO_MAX	256

#define	ISO_PORT_RESERVED		1024
#define	ISO_PORT_USERRESERVED	5000
/*
 * Port/socket numbers: standard network functions
 * NOT PRESENTLY USED
 */
#define	ISO_PORT_MAINT		501
#define	ISO_PORT_ECHO		507
#define	ISO_PORT_DISCARD	509
#define	ISO_PORT_SYSTAT		511
#define	ISO_PORT_NETSTAT	515
/*
 * Port/socket numbers: non-standard application functions
 */
#define ISO_PORT_LOGIN		513
/*
 * Port/socket numbers: public use
 */
#define ISO_PORT_PUBLIC		1024		/* high bit set --> public */

/*
 *	Network layer protocol identifiers
 */
#define ISO8473_CLNP	0x81
#define	ISO9542_ESIS	0x82
#define ISO9542X25_ESIS	0x8a
#define ISO10589_ISIS		0x83
#define ISO8878A_CONS		0x84
#define ISO10747_IDRP		0x85


/* The following looks like a sockaddr
 * to facilitate using tree lookup routines */
struct iso_addr {
	u_char	isoa_len;						/* length (in bytes) */
	char	isoa_genaddr[20];				/* general opaque address */
};

struct sockaddr_iso {
	u_char	 			siso_len;			/* length */
	u_char	 			siso_family;		/* family */
	u_char				siso_plen;			/* presentation selector length */
	u_char				siso_slen;			/* session selector length */
	u_char				siso_tlen;			/* transport selector length */
	struct 	iso_addr	siso_addr;			/* network address */
	u_char				siso_pad[6];		/* space for gosip v2 sels */
											/* makes struct 32 bytes long */
};
#define siso_nlen siso_addr.isoa_len
#define siso_data siso_addr.isoa_genaddr

#define TSEL(s) ((caddr_t)((s)->siso_data + (s)->siso_nlen))

#define SAME_ISOADDR(a, b) \
	(bcmp((a)->siso_data, (b)->siso_data, (unsigned)(a)->siso_nlen)==0)
/*
 * The following are specific values for siso->siso_data[0],
 * otherwise known as the AFI:
 */
#define	AFI_37		0x37	/* bcd of "37" */
#define AFI_OSINET	0x47	/* bcd of "47" */
#define AFI_RFC986	0x47	/* bcd of "47" */
#define	AFI_SNA		0x00	/* SubNetwork Address; invalid really...*/

#ifdef KERNEL
extern int iso_netmatch();
extern int iso_hash(); 
extern int iso_addrmatch();
extern struct iso_ifaddr *iso_iaonnetof();
extern	struct domain isodomain;
extern	struct protosw isosw[];

#else
/* user utilities definitions from the iso library */

struct iso_addr *iso_addr(const char *);
char *iso_ntoa(const struct iso_addr *);

/* THESE DON'T EXIST YET */
struct hostent *iso_gethostbyname(), *iso_gethostbyaddr();

#endif /* KERNEL */

#endif // NET_ISO_H
