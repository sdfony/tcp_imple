#ifndef HP300_HP300_ISR_H
#define HP300_HP300_ISR_H

struct isr {
	struct	isr *isr_forw;
	struct	isr *isr_back;
	int	(*isr_intr)();
	int	isr_arg;
	int	isr_ipl;
};

#define	NISR		3
#define	ISRIPL(x)	((x) - 3)

#endif  // HP300_HP300_ISR_H
