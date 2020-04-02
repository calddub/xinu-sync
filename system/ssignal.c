/* signal.c - signal */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  signal  -  Signal a semaphore, releasing a process if one is waiting
 *------------------------------------------------------------------------
 */
syscall	ssignal(
	  sid32		sem1,		/* ID of semaphore to signal	*/
	  sid32		sem2		/* ID of semaphore to signal	*/
	)
{
	intmask mask;			/* Saved interrupt mask		*/
	struct	sentry *sem1ptr;		/* Ptr to sempahore table entry	*/
	struct	sentry *sem2ptr;		/* Ptr to sempahore table entry	*/

	mask = disable();
	//if (isbadsem(sem)) {
	if (isbadsem(sem1) || isbadsem(sem2) ) {
		restore(mask);
		return SYSERR;
	}

	if( sem1 == sem2 ) {
		restore(mask);
		return SYSERR;
	}

	sem1ptr= &semtab[sem1];
	sem2ptr= &semtab[sem2];

	//if (semptr->sstate == S_FREE) {
	if (sem1ptr->sstate == S_FREE || sem2ptr->sstate == S_FREE ) {
		restore(mask);
		return SYSERR;
	}

	if ((sem1ptr->scount++) < 0) {	/* Release a waiting process */
		ready(dequeue(sem1ptr->squeue));
	}
	if ((sem2ptr->scount++) < 0) {	/* Release a waiting process */
		ready(dequeue(sem2ptr->squeue));
	}
	restore(mask);
	return OK;
}
