#include "../sys/param.h"
#include "../sys/systm.h"
#include "../sys/ioctl.h"
#include "../sys/proc.h"
#include "../sys/file.h"
//#include "../sys/conf.h"
#include "../sys/dkstat.h"
#include "../sys/uio.h"
#include "../sys/kernel.h"
//#include "../sys/vnode.h"
#include "../sys/syslog.h"
#include "../sys/tty.h"
//#include <vm/vm.h>

/*#define	TTYDEFCHARS*/
/*#include <sys/tty.h>*/
/*#undef	TTYDEFCHARS*/

static int	proc_compare __P((struct proc *p1, struct proc *p2));
static int	ttnread __P((struct tty *));
static void	ttyblock __P((struct tty *tp));
static void	ttyecho __P((int, struct tty *tp));
static void	ttyrubo __P((struct tty *, int));

/* Symbolic sleep message strings. */
char ttclos[]	= "ttycls";
char ttopen[]	= "ttyopn";
char ttybg[]	= "ttybg";
char ttybuf[]	= "ttybuf";
char ttyin[]	= "ttyin";
char ttyout[]	= "ttyout";

/*
 * Table with character classes and parity. The 8th bit indicates parity,
 * the 7th bit indicates the character is an alphameric or underscore (for
 * ALTWERASE), and the low 6 bits indicate delay type.  If the low 6 bits
 * are 0 then the character needs no special processing on output; classes
 * other than 0 might be translated or (not currently) require delays.
 */
#define	E	0x00	/* Even parity. */
#define	O	0x80	/* Odd parity. */
#define	PARITY(c)	(char_type[c] & O)

#define	ALPHA	0x40	/* Alpha or underscore. */
#define	ISALPHA(c)	(char_type[(c) & TTY_CHARMASK] & ALPHA)

#define	CCLASSMASK	0x3f
#define	CCLASS(c)	(char_type[c] & CCLASSMASK)

#define	BS	BACKSPACE
#define	CC	CONTROL
#define	CR	RETURN
#define	NA	ORDINARY | ALPHA
#define	NL	NEWLINE
#define	NO	ORDINARY
#define	TB	TAB
#define	VT	VTAB

char const char_type[] = {
	E|CC, O|CC, O|CC, E|CC, O|CC, E|CC, E|CC, O|CC,	/* nul - bel */
	O|BS, E|TB, E|NL, O|CC, E|VT, O|CR, O|CC, E|CC, /* bs - si */
	O|CC, E|CC, E|CC, O|CC, E|CC, O|CC, O|CC, E|CC, /* dle - etb */
	E|CC, O|CC, O|CC, E|CC, O|CC, E|CC, E|CC, O|CC, /* can - us */
	O|NO, E|NO, E|NO, O|NO, E|NO, O|NO, O|NO, E|NO, /* sp - ' */
	E|NO, O|NO, O|NO, E|NO, O|NO, E|NO, E|NO, O|NO, /* ( - / */
	E|NA, O|NA, O|NA, E|NA, O|NA, E|NA, E|NA, O|NA, /* 0 - 7 */
	O|NA, E|NA, E|NO, O|NO, E|NO, O|NO, O|NO, E|NO, /* 8 - ? */
	O|NO, E|NA, E|NA, O|NA, E|NA, O|NA, O|NA, E|NA, /* @ - G */
	E|NA, O|NA, O|NA, E|NA, O|NA, E|NA, E|NA, O|NA, /* H - O */
	E|NA, O|NA, O|NA, E|NA, O|NA, E|NA, E|NA, O|NA, /* P - W */
	O|NA, E|NA, E|NA, O|NO, E|NO, O|NO, O|NO, O|NA, /* X - _ */
	E|NO, O|NA, O|NA, E|NA, O|NA, E|NA, E|NA, O|NA, /* ` - g */
	O|NA, E|NA, E|NA, O|NA, E|NA, O|NA, O|NA, E|NA, /* h - o */
	O|NA, E|NA, E|NA, O|NA, E|NA, O|NA, O|NA, E|NA, /* p - w */
	E|NA, O|NA, O|NA, E|NO, O|NO, E|NO, E|NO, O|CC, /* x - del */
	/*
	 * Meta chars; should be settable per character set;
	 * for now, treat them all as normal characters.
	 */
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
	NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,
};
#undef	BS
#undef	CC
#undef	CR
#undef	NA
#undef	NL
#undef	NO
#undef	TB
#undef	VT

/* Macros to clear/set/test flags. */
#define	SET(t, f)	(t) |= (f)
#define	CLR(t, f)	(t) &= ~(f)
#define	ISSET(t, f)	((t) & (f))

/*
 * Initial open of tty, or (re)entry to standard tty line discipline.
 */
int
ttyopen(device, tp)
	dev_t device;
	register struct tty *tp;
{
    return 0;
}

/*
 * Handle close() on a tty line: flush and set to initial state,
 * bumping generation number so that pending read/write calls
 * can detect recycling of the tty.
 */
int
ttyclose(tp)
	register struct tty *tp;
{
    return 0;
}

#define	FLUSHQ(q) {							\
	if ((q)->c_cc)							\
		ndflush(q, (q)->c_cc);					\
}

/* Is 'c' a line delimiter ("break" character)? */
#define	TTBREAKC(c)							\
	((c) == '\n' || ((c) == cc[VEOF] ||				\
	(c) == cc[VEOL] || (c) == cc[VEOL2]) && (c) != _POSIX_VDISABLE)


/*
 * Process input of a single character received on a tty.
 */
int
ttyinput(c, tp)
	register int c;
	register struct tty *tp;
{
    return 0;
}

/*
 * Output a single character on a tty, doing output processing
 * as needed (expanding tabs, newline processing, etc.).
 * Returns < 0 if succeeds, otherwise returns char to resend.
 * Must be recursive.
 */
int
ttyoutput(c, tp)
	register int c;
	register struct tty *tp;
{
    return 0;
}

/*
 * Ioctls for all tty devices.  Called after line-discipline specific ioctl
 * has been called to do discipline-specific functions and/or reject any
 * of these ioctl commands.
 */
/* ARGSUSED */
int
ttioctl(tp, cmd, data, flag)
	register struct tty *tp;
	int cmd, flag;
	void *data;
{
    return 0;
}

int
ttselect(device, rw, p)
	dev_t device;
	int rw;
	struct proc *p;
{
    return 0;
}

static int
ttnread(tp)
	struct tty *tp;
{
    return 0;
}

/*
 * Wait for output to drain.
 */
int
ttywait(tp)
	register struct tty *tp;
{
    return 0;
}

/*
 * Flush if successfully wait.
 */
int
ttywflush(tp)
	struct tty *tp;
{
    return 0;
}

/*
 * Flush tty read and/or write queues, notifying anyone waiting.
 */
void
ttyflush(tp, rw)
	register struct tty *tp;
	int rw;
{

}

/*
 * Copy in the default termios characters.
 */
void
ttychars(tp)
	struct tty *tp;
{
}

/*
 * Send stop character on input overflow.
 */
static void
ttyblock(tp)
	register struct tty *tp;
{
}

void
ttrstrt(tp_arg)
	void *tp_arg;
{
}

int
ttstart(tp)
	struct tty *tp;
{
    return 0;
}

/*
 * "close" a line discipline
 */
int
ttylclose(tp, flag)
	struct tty *tp;
	int flag;
{
    return 0;
}

/*
 * Handle modem control transition on a tty.
 * Flag indicates new state of carrier.
 * Returns 0 if the line should be turned off, otherwise 1.
 */
int
ttymodem(tp, flag)
	register struct tty *tp;
	int flag;
{
    return 0;
}

/*
 * Default modem control routine (for other line disciplines).
 * Return argument flag, to turn off device on carrier drop.
 */
int
nullmodem(tp, flag)
	register struct tty *tp;
	int flag;
{
    return 0;
}

/*
 * Reinput pending characters after state switch
 * call at spltty().
 */
void
ttypend(tp)
	register struct tty *tp;
{
}

/*
 * Process a read call on a tty device.
 */
int
ttread(tp, uio, flag)
	register struct tty *tp;
	struct uio *uio;
	int flag;
{
    return 0;
}

/*
 * Check the output queue on tp for space for a kernel message (from uprintf
 * or tprintf).  Allow some space over the normal hiwater mark so we don't
 * lose messages due to normal flow control, but don't let the tty run amok.
 * Sleeps here are not interruptible, but we return prematurely if new signals
 * arrive.
 */
int
ttycheckoutq(tp, wait)
	register struct tty *tp;
	int wait;
{
    return 0;
}

/*
 * Process a write call on a tty device.
 */
int
ttwrite(tp, uio, flag)
	register struct tty *tp;
	register struct uio *uio;
	int flag;
{
    return 0;
}

/*
 * Rubout one character from the rawq of tp
 * as cleanly as possible.
 */
void
ttyrub(c, tp)
	register int c;
	register struct tty *tp;
{
}

/*
 * Back over cnt characters, erasing them.
 */
static void
ttyrubo(tp, cnt)
	register struct tty *tp;
	int cnt;
{
}

/*
 * ttyretype --
 *	Reprint the rawq line.  Note, it is assumed that c_cc has already
 *	been checked.
 */
void
ttyretype(tp)
	register struct tty *tp;
{
}

/*
 * Echo a typed character to the terminal.
 */
static void
ttyecho(c, tp)
	register int c;
	register struct tty *tp;
{
}

/*
 * Wake up any readers on a tty.
 */
void
ttwakeup(tp)
	register struct tty *tp;
{
}

/*
 * Look up a code for a specified speed in a conversion table;
 * used by drivers to map software speed values to hardware parameters.
 */
int
ttspeedtab(speed, table)
	int speed;
	register struct speedtab *table;
{
    return 0;
}

/*
 * Set tty hi and low water marks.
 *
 * Try to arrange the dynamics so there's about one second
 * from hi to low water.
 *
 */
void
ttsetwater(tp)
	struct tty *tp;
{
    
}

/*
 * Report on state of foreground process group.
 */
void
ttyinfo(tp)
	register struct tty *tp;
{
}

/*
 * Returns 1 if p2 is "better" than p1
 *
 * The algorithm for picking the "interesting" process is thus:
 *
 *	1) Only foreground processes are eligible - implied.
 *	2) Runnable processes are favored over anything else.  The runner
 *	   with the highest cpu utilization is picked (p_estcpu).  Ties are
 *	   broken by picking the highest pid.
 *	3) The sleeper with the shortest sleep time is next.  With ties,
 *	   we pick out just "short-term" sleepers (P_SINTR == 0).
 *	4) Further ties are broken by picking the highest pid.
 */
#define ISRUN(p)	(((p)->p_stat == SRUN) || ((p)->p_stat == SIDL))
#define TESTAB(a, b)    ((a)<<1 | (b))
#define ONLYA   2
#define ONLYB   1
#define BOTH    3

static int
proc_compare(p1, p2)
	register struct proc *p1, *p2;
{
    return 0;
}

/*
 * Output char to tty; console putchar style.
 */
int
tputchar(c, tp)
	int c;
	struct tty *tp;
{
    return 0;
}

/*
 * Sleep on chan, returning ERESTART if tty changed while we napped and
 * returning any errors (e.g. EINTR/ETIMEDOUT) reported by tsleep.  If
 * the tty is revoked, restarting a pending call will redo validation done
 * at the start of the call.
 */
int
ttysleep(tp, chan, pri, wmesg, timo)
	struct tty *tp;
	void *chan;
	int pri, timo;
	char *wmesg;
{
    return 0;
}
