#ifndef NET_NETISR_H
#define NET_NETISR_H

#include "..\hp300\include\cpu.h"

#if defined(vax) || defined(tahoe)
#define	setsoftnet()	mtpr(SIRR, 12)
#endif

/*
 * Each ``pup-level-1'' input queue has a bit in a ``netisr'' status
 * word which is used to de-multiplex a single software
 * interrupt used for scheduling the network code to calls
 * on the lowest level routine of each protocol.
 */
#define	NETISR_RAW	0		/* same as AF_UNSPEC */
#define	NETISR_IP	2		/* same as AF_INET */
#define	NETISR_IMP	3		/* same as AF_IMPLINK */
#define	NETISR_NS	6		/* same as AF_NS */
#define	NETISR_ISO	7		/* same as AF_ISO */
#define	NETISR_CCITT	10		/* same as AF_CCITT */
#define	NETISR_ARP	18		/* same as AF_LINK */

#define	schednetisr(anisr)	{ netisr |= 1<<(anisr); setsoftnet(); }

#ifdef i386
/* XXX Temporary -- soon to vanish - wfj */
#define	NETISR_SCLK	11		/* softclock */
#define	NETISR_AST	12		/* ast -- resched */

#undef	schednetisr
#define	schednetisr(anisr)	{\
	if(netisr == 0) { \
		softem++; \
	} \
	netisr |= 1<<(anisr); \
}
#ifndef LOCORE
#ifdef KERNEL
int	softem;	
#endif
#endif
#endif /* i386 */

#ifndef LOCORE
#ifdef KERNEL
int	netisr;				/* scheduling bits for network */
#endif
#endif

#endif  // NET_NETISR_H
