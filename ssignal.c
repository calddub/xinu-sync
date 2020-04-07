/*----------------------------------------------*/
// Name     : Nicholas Sabata and David Moore	//
// Class    : 8530-001							//
// Assignment # : 2								//
// Due Date     : April 7th, 2020				//
/*----------------------------------------------*/
/* ssignal.c - ssignal */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ssignal - Signal a semaphore, releasing a process if one is waiting
 * "Simultaneous Signal"
 *------------------------------------------------------------------------
 */
syscall	ssignal(
	  sid32 sem1,				/* ID of semaphore 1 to signal */
	  sid32 sem2				/* ID of semaphore 2 to signal */
	)
{
	intmask mask;				/* Saved interrupt mask */
	struct sentry *sem1ptr;		/* Ptr to semaphore 1 table entry */
	struct sentry *sem2ptr;		/* Ptr to semaphore 2 table entry */

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
	sem1ptr= &semtab[sem1];
	sem2ptr= &semtab[sem2];

	// Check if either semaphore is currently in the free state
	if ( sem1ptr->sstate == S_FREE || sem2ptr->sstate == S_FREE ) {
		restore(mask);
		return SYSERR;
	}

	// Process that has been waiting longest on each semaphore gets moved
	// from the wait state to the ready state
	resched_cntl(DEFER_START);				/* Start deferring scheduling */
	if ( (sem1ptr->scount++) < 0 ) {		/* Increment sem 1 count */
		ready(dequeue(sem1ptr->squeue));	/* Release a sem 1 waiting process */
	}
	if ( (sem2ptr->scount++) < 0 ) {		/* Increment sem 2 count */
		ready(dequeue(sem2ptr->squeue));	/* Release a sem 2 waiting process */
	}
	resched_cntl(DEFER_STOP);				/* Stop deferring scheduling */
	
	// Interrupt mask is restored and process returns from system call
	restore(mask);
	return OK;
}
