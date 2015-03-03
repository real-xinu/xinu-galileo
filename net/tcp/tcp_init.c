/* tcp_init.c  -  tcp_init */

#include <xinu.h>

struct	tcb	tcbtab[Ntcp];		/* The TCB table		*/
struct	tcpcontrol	Tcp;		/* TCP Control			*/
extern	process tcp_out(void);

/*------------------------------------------------------------------------
 *  tcp_init  -  Initialize TCB Table
 *------------------------------------------------------------------------
 */
int32	tcp_init(void)
{
	struct	tcb	*tcbptr;	/* Ptr to TCB entry	*/
	int32	i;			/* For loop index	*/

	tminit();
	mqinit();

	for(i = 0; i < Ntcp; i++) {

		/* Point to correct TCB entry */

		tcbptr = &tcbtab[i];

		/* Initialize TCB semaphores */

		tcbptr->tcb_mutex  = semcreate (1);
		tcbptr->tcb_rblock = semcreate (0);
		tcbptr->tcb_wblock = semcreate (0);

		tcbptr->tcb_lq = mqcreate (15);

		/* Start the refernce count at zero and mark the TCB free */

		tcbptr->tcb_ref = 0;
		tcbptr->tcb_state = TCB_FREE;
	}

	/* Initialize values in the Tcp strcuture */

	Tcp.tcpmutex = semcreate (1);
	Tcp.tcpnextport = 33000;

	Tcp.tcpcmdq = mqcreate (10);

	resume(create(tcp_out, 8192, 500, "tcp_out", 0, NULL));

	return OK;
}
