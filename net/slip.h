#ifndef NET_SLIP_H
#define NET_SLIP_H

#include "..\sys\ioccom.h"
/* Ioctls operating on SLIP ttys. */
#define	SLIOCGUNIT	_IOR('t', 88, int)	/* get slip unit number */

/*
 * Definitions of the pseudo-link-level header attached to slip
 * packets grabbed by the packet filter (bpf) traffic monitor.
 */
#define	SLIP_HDRLEN	16		/* BPF SLIP header length */

/* Offsets into BPF SLIP header. */
#define	SLX_DIR		0		/* direction; see below */
#define	SLX_CHDR	1		/* compressed header data */
#define	CHDR_LEN	15		/* length of compressed header data */

#define	SLIPDIR_IN	0		/* incoming */
#define	SLIPDIR_OUT	1		/* outgoing */

#endif  // NET_SLIP_H
