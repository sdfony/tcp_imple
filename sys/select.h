#ifndef SYS_SELECT_H_
#define	SYS_SELECT_H_

/*
 * Used to maintain information about processes that wish to be
 * notified when I/O becomes possible.
 */
struct selinfo {
	pid_t	si_pid;		/* process to be notified */
	short	si_flags;	/* see below */
};
#define	SI_COLL	0x0001		/* collision occurred */

#ifdef KERNEL
struct proc;

void	selrecord(struct proc *selector, struct selinfo *));
void	selwakeup(struct selinfo *));
#endif

#endif /* SYS_SELECT_H_ */
