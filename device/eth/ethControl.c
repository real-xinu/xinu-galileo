/* ethControl.c -  ethControl */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethControl - implement control function for an Ethernet device
 *------------------------------------------------------------------------
 */
devcall	ethControl (
	 struct	dentry	*devptr,			/* entry in device switch table	*/
	 int32	func,						/* control function				*/
	 int32	arg1,						/* argument 1, if needed		*/
	 int32	arg2						/* argument 2, if needed		*/
	)
{
	struct	ether	*ethptr; 		/* ptr to control block	*/

	ethptr = &ethertab[devptr->dvminor];

	return ethptr->ethControl(ethptr, func, arg1, arg2);
}
