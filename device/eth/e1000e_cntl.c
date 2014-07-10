/* e1000e_cntl.c - e1000e_cntl */

#include <xinu.h>

/*------------------------------------------------------------------------
 * e1000e_cntl - implement control function for an E1000E device
 *------------------------------------------------------------------------
 */
devcall	e1000e_cntl(
	struct	ether *ethptr,		/* entry in device switch table	*/
	int32	func,			/* control function		*/
	int32	arg1,			/* argument 1, if needed 	*/
	int32	arg2			/* argument 2, if needed	*/
	)
{
	switch (func) {

		/* Get MAC address */

		case ETH_CTRL_GET_MAC:
			memcpy((byte *)arg1, ethptr->devAddress, 
					ETH_ADDR_LEN);
			break;

		default:
			return SYSERR;
	}

	return OK;
}
