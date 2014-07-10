/* ethInterrupt.c - ethInterrupt */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethInterrupt - decode and handle interrupt from an Ethernet device
 *------------------------------------------------------------------------
 */
interrupt ethInterrupt(void) {

	struct  dentry  *devptr;        /* address of device control blk*/
	struct 	ether 	*ethptr;	/* ptr to control block		*/

	/* Initialize structure pointers */
	devptr = (struct dentry *) &devtab[ETHER0];
	
	/* Obtain a pointer to the tty control block */
	ethptr = &ethertab[devptr->dvminor];
	
	ethptr->ethInterrupt(ethptr);

	return;
}
