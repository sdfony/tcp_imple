#ifndef SYS_CALLOUT_H
#define SYS_CALLOUT_H

struct callout {
	struct	callout *c_next;		/* next callout in queue */
	void	*c_arg;				/* function argument */
	void	(*c_func) __P((void *));	/* function to call */
	int	c_time;				/* ticks to the event */
};

//#ifdef KERNEL
struct	callout *callfree, *callout, calltodo;
int	ncallout;
//#endif

#endif  // SYS_CALLOUT_H
