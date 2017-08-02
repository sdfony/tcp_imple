#include "../sys/param.h"
#include "../sys/systm.h"
#include "../sys/dkstat.h"
#include "../sys/callout.h"
#include "../sys/kernel.h"
#include "../sys/proc.h"
#include "../sys/resourcevar.h"

/*#include <machine/cpu.h>*/

/*#ifdef GPROF*/
/*#include <sys/gmon.h>*/
/*#endif*/

/*
 * Clock handling routines.
 *
 * This code is written to operate with two timers that run independently of
 * each other.  The main clock, running hz times per second, is used to keep
 * track of real time.  The second timer handles kernel and user profiling,
 * and does resource use estimation.  If the second timer is programmable,
 * it is randomized to avoid aliasing between the two clocks.  For example,
 * the randomization prevents an adversary from always giving up the cpu
 * just before its quantum expires.  Otherwise, it would never accumulate
 * cpu ticks.  The mean frequency of the second timer is stathz.
 *
 * If no second timer exists, stathz will be zero; in this case we drive
 * profiling and statistics off the main clock.  This WILL NOT be accurate;
 * do not do it unless absolutely necessary.
 *
 * The statistics clock may (or may not) be run at a higher rate while
 * profiling.  This profile clock runs at profhz.  We require that profhz
 * be an integral multiple of stathz.
 *
 * If the statistics clock is running fast, it must be divided by the ratio
 * profhz/stathz for statistics.  (For profiling, every tick counts.)
 */

/*
 * TODO:
 *	allocate more timeout table slots when table overflows.
 */

/*
 * Bump a timeval by a small number of usec's.
 */
#define BUMPTIME(t, usec) { \
	register volatile struct timeval *tp = (t); \
	register long us; \
 \
	tp->tv_usec = us = tp->tv_usec + (usec); \
	if (us >= 1000000) { \
		tp->tv_usec = us - 1000000; \
		tp->tv_sec++; \
	} \
}

int	stathz;
int	profhz;
int	profprocs;
int	ticks;
static int psdiv, pscnt;	/* prof => stat divider */
int	psratio;		/* ratio: prof / stat */

volatile struct	timeval time;
volatile struct	timeval mono_time;

/*
 * Initialize clock frequencies and start both clocks running.
 */
void
initclocks()
{
}

/*
 * The real-time timer, interrupting hz times per second.
 */
void
hardclock(frame)
	register struct clockframe *frame;
{
}

/*
 * Software (low priority) clock interrupt.
 * Run periodic events from timeout queue.
 */
/*ARGSUSED*/
void
softclock()
{
}

/*
 * timeout --
 *	Execute a function after a specified length of time.
 *
 * untimeout --
 *	Cancel previous timeout function call.
 *
 *	See AT&T BCI Driver Reference Manual for specification.  This
 *	implementation differs from that one in that no identification
 *	value is returned from timeout, rather, the original arguments
 *	to timeout are used to identify entries for untimeout.
 */
void
timeout(ftn, arg, ticks)
	void (*ftn) __P((void *));
	void *arg;
	register int ticks;
{
}

void
untimeout(ftn, arg)
	void (*ftn) __P((void *));
	void *arg;
{
}

/*
 * Compute number of hz until specified time.  Used to
 * compute third argument to timeout() from an absolute time.
 */
int
hzto(tv)
	struct timeval *tv;
{
    return 0;
}

/*
 * Start profiling on a process.
 *
 * Kernel profiling passes proc0 which never exits and hence
 * keeps the profile clock running constantly.
 */
void
startprofclock(p)
	register struct proc *p;
{
}

/*
 * Stop profiling on a process.
 */
void
stopprofclock(p)
	register struct proc *p;
{
}

int	dk_ndrive = DK_NDRIVE;

/*
 * Statistics clock.  Grab profile sample, and if divider reaches 0,
 * do process and kernel statistics.
 */
void
statclock(frame)
	register struct clockframe *frame;
{
}

/*
 * Return information about system clocks.
 */
sysctl_clockrate(where, sizep)
	register char *where;
	size_t *sizep;
{
    return 0;
}
