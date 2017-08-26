#include "../sys/param.h"
#include "../sys/ioctl.h"
#include "../sys/tty.h"

char cwaiting;
struct cblock *cfree, *cfreelist;
int cfreecount, nclist;

void
clist_init()
{

	/*
	 * Body deleted.
	 */
	return;
}

getc(a1)
	struct clist *a1;
{

	/*
	 * Body deleted.
	 */
	return ((char)0);
}

q_to_b(a1, a2, a3)
	struct clist *a1;
	char *a2;
	int a3;
{

	/*
	 * Body deleted.
	 */
	return (0);
}

ndqb(a1, a2)
	struct clist *a1;
	int a2;
{

	/*
	 * Body deleted.
	 */
	return (0);
}

void
ndflush(a1, a2)
	struct clist *a1;
	int a2;
{

	/*
	 * Body deleted.
	 */
	return;
}

putc(a1, a2)
	char a1;
	struct clist *a2;
{

	/*
	 * Body deleted.
	 */
	return (0);
}

b_to_q(a1, a2, a3)
	char *a1;
	int a2;
	struct clist *a3;
{

	/*
	 * Body deleted.
	 */
	return (0);
}

char *
nextc(a1, a2, a3)
	struct clist *a1;
	char *a2;
	int *a3;
{

	/*
	 * Body deleted.
	 */
	return ((char *)0);
}

unputc(a1)
	struct clist *a1;
{

	/*
	 * Body deleted.
	 */
	return ((char)0);
}

void
catq(a1, a2)
	struct clist *a1, *a2;
{

	/*
	 * Body deleted.
	 */
	return;
}
