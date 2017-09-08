#ifndef _ENDIAN_H_
#define	_ENDIAN_H_

/*
 * Define the order of 32-bit words in 64-bit words.
 */
#define _QUAD_HIGHWORD 0
#define _QUAD_LOWWORD 1

#ifndef _POSIX_SOURCE
/*
 * Definitions for byte order, according to byte significance from low
 * address to high.
 */
#define	LITTLE_ENDIAN	1234	/* LSB first: i386, vax */
#define	BIG_ENDIAN	4321	/* MSB first: 68000, ibm, net */
#define	PDP_ENDIAN	3412	/* LSB first in word, MSW first in long */

#define	BYTE_ORDER	LITTLE_ENDIAN

#include "../../sys/cdefs.h"

__BEGIN_DECLS
inline unsigned long	htonl __P((unsigned long l))
{
    unsigned char *c = (unsigned char *)&l;

    return c[0] >> 24 | c[1] >> 8 | c[2] << 8 | c[3] << 24;
}

inline unsigned short	htons __P((unsigned short s))
{
    unsigned char *c = (unsigned char *)&s;

    return c[0] >> 8 | c[1] << 8;
}

inline unsigned long	ntohl __P((unsigned long l))
{
    unsigned char *c = (unsigned char *)&l;

    return c[0] >> 24 | c[1] >> 8 | c[2] << 8 | c[3] << 24;
}

inline unsigned short	ntohs __P((unsigned short s))
{
    unsigned char *c = (unsigned char *)&s;

    return c[0] >> 8 | c[1] << 8;
}
__END_DECLS

/*
 * Macros for network/external number representation conversion.
 */
#if BYTE_ORDER == BIG_ENDIAN && !defined(lint)
#define	ntohl(x)	(x)
#define	ntohs(x)	(x)
#define	htonl(x)	(x)
#define	htons(x)	(x)

#define	NTOHL(x)	(x)
#define	NTOHS(x)	(x)
#define	HTONL(x)	(x)
#define	HTONS(x)	(x)

#else

#define	NTOHL(x)	(x) = ntohl((u_long)x)
#define	NTOHS(x)	(x) = ntohs((u_short)x)
#define	HTONL(x)	(x) = htonl((u_long)x)
#define	HTONS(x)	(x) = htons((u_short)x)
#endif
#endif /* !_POSIX_SOURCE */
#endif /* !_ENDIAN_H_ */
