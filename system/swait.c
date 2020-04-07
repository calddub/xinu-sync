/*----------------------------------------------*/
// Name     : Nicholas Sabata and David Moore	//
// Class    : 8530-001							//
// Assignment # : 2								//
// Due Date     : April 7th, 2020				//
/*----------------------------------------------*/
/* swait.c - swait */

#include <xinu.h>

/*------------------------------------------------------------------------
 * swait - Cause current process to wait on a semaphore
 * "Simultaneous Wait"
 *------------------------------------------------------------------------
 */
syscall	swait(
	  sid32 sem1,				/* Semaphore 1 on which to wait  */
	  sid32 sem2				/* Semaphore 2 on which to wait  */
	)
{
	intmask mask;				/* Saved interrupt mask */
	struct procent *prptr;		/* Ptr to process' table entry */
	struct sentry *sem1ptr;		/* Ptr to semaphore 1 table entry */
	struct sentry *sem2ptr;		/* Ptr to semaphore 2 table entry */

	// First disable interrupts
	mask = disable();
	
	// Verify that semaphore args are reasonable (failure returns SYSERR)
	// (Semaphore args should not be the same)
	if( sem1 == sem2 ) {
		restore(mask);
		return SYSERR;
	}
	// (Semaphore args should exist and be allocated)
	if ( isbadsem(sem1) || isbadsem(sem2) ) {
		restore(mask);
		return SYSERR;
	}

	// Assign semaphore pointers
	sem1ptr = &semtab[sem1];
	sem2ptr = &semtab[sem2];
	
	// Check if either semaphore is currently in the free state
	if ( sem1ptr->sstate == S_FREE || sem2ptr->sstate == S_FREE ) {
		restore(mask);
		return SYSERR;
	}
	
	// Check if both semaphores have a count greater than zero
	if( sem1ptr->scount > 0 && sem2ptr->scount > 0 ) {
		--(sem1ptr->scount);				/* Decrement sem 1 count */
		--(sem2ptr->scount);				/* Decrement sem 2 count */
		
	// Case where sem 1 count is not greater than zero
	// (Process will block in the wait state)
	} else if( sem1ptr->scount <= 0 ) { 	/* If caller must block	*/
		--(sem1ptr->scount);				/* Decrement sem 1 count */
		prptr = &proctab[currpid];
		prptr->prstate = PR_WAIT;			/* Set state to waiting	*/
		prptr->prsem = sem1;				/* Record semaphore ID */
		enqueue(currpid,sem1ptr->squeue);	/* Enqueue on semaphore */
		resched();							/* Reschedule */
		
	// Case where sem 2 count is not greater than zero
	// (Process will block in the wait state)
	} else if( sem2ptr->scount <= 0 ) { 	/* If caller must block	*/
		--(sem2ptr->scount);				/* Decrement sem 2 count */
		prptr = &proctab[currpid];
		prptr->prstate = PR_WAIT;			/* Set state to waiting	*/
		prptr->prsem = sem2;				/* Record semaphore ID */
		enqueue(currpid,sem2ptr->squeue);	/* Enqueue on semaphore */
		resched();							/* Reschedule */
	}

	// Interrupt mask is restored and process returns from system call
	restore(mask);
	return OK;
}
