#ifndef	_SYS_IOCTL_H_
#define	_SYS_IOCTL_H_

#include "ttycom.h"
#include "ioccom.h"
#include "filio.h"
#include "sockio.h"
#include "cdefs.h"

/*
 * Pun for SunOS prior to 3.2.  SunOS 3.2 and later support TIOCGWINSZ
 * and TIOCSWINSZ (yes, even 3.2-3.5, the fact that it wasn't documented
 * nonwithstanding).
 */
struct ttysize {
	unsigned short	ts_lines;
	unsigned short	ts_cols;
	unsigned short	ts_xxx;
	unsigned short	ts_yyy;
};
#define	TIOCGSIZE	TIOCGWINSZ
#define	TIOCSSIZE	TIOCSWINSZ

#ifndef KERNEL

__BEGIN_DECLS
int	ioctl __P((int, unsigned long, ...));
__END_DECLS
#endif /* !KERNEL */


#endif /* !_SYS_IOCTL_H_ */

/*
 * Keep outside _SYS_IOCTL_H_
 * Compatability with old terminal driver
 *
 * Source level -> #define USE_OLD_TTY
 * Kernel level -> options COMPAT_43 or COMPAT_SUNOS
 */
//#if defined(USE_OLD_TTY) || defined(COMPAT_43) || defined(COMPAT_SUNOS)
//#include <sys/ioctl_compat.h>
//#endif
