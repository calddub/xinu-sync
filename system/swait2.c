/* wait.c - wait */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  wait  -  Cause current process to wait on a semaphore
 *------------------------------------------------------------------------
 */
syscall	swait2(
	  sid32		sem1,		/* Semaphore on which to wait  */
	  sid32		sem2		/* Semaphore on which to wait  */
	)
{
	intmask mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	struct	sentry *sem1ptr;	/* Ptr to sempahore table entry	*/
	struct	sentry *sem2ptr;	/* Ptr to sempahore table entry	*/

	mask = disable();
	//if (isbadsem(sem)) {
	if( sem1 == sem2 ) {
		restore(mask);
		return SYSERR;
	}

	if ( isbadsem(sem1) || isbadsem(sem2)) {
		restore(mask);
		return SYSERR;
	}

	sem1ptr = &semtab[sem1];
	sem2ptr = &semtab[sem2];
	//if (semptr->sstate == S_FREE) {
	if (sem1ptr->sstate == S_FREE || sem2ptr->sstate == S_FREE) {
		restore(mask);
		return SYSERR;
	}

	// Check that sem1->cnt > 0 && sem2->cnt > 0 { // easy case
	//   Do similar stuff to regular wait() call)
	// } case 2 sem1->cnt > 0 && sem2->cnt <=0 {
	// } case 3 sem1->cnt <= 0 && sem2->cnt > 0 {
	// } case 4 sem1->cnt <= 0 && sem2->cnt <= 0 {
	// }
	/*
	if (--(semptr->scount) < 0) {		// If caller must block	
		prptr = &proctab[currpid];
		prptr->prstate = PR_WAIT;	// Set state to waiting
		prptr->prsem = sem;		// Record semaphore ID
		enqueue(currpid,semptr->squeue);// Enqueue on semaphore
		resched();			//   and reschedule
	}
	*/

	restore(mask);
	return OK;
}
