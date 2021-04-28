/* ethcontrol.c - ethcontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 * ethcontrol - implement control function for a quark ethernet device
 *------------------------------------------------------------------------
 */
devcall	ethcontrol (
	struct 	dentry *devptr, 	/* entry in device switch table */
	int32	func,			/* control function		*/
	int32	arg1,			/* argument 1, if needed	*/
	int32	arg2			/* argument 2, if needed	*/
	)
{
	struct	ethcblk *ethptr;	/* Ethertab entry pointer	*/
	int32	retval = OK;		/* Return value of cntl function*/

	ethptr = &ethertab[devptr->dvminor];

	switch (func) {

		/* Get MAC address */

		case ETH_CTRL_GET_MAC:
			memcpy((byte *)arg1, ethptr->devAddress,
					ETH_ADDR_LEN);
			break;

		/* Add a multicast address */

		case ETH_CTRL_ADD_MCAST:
			retval = ethmcast_add(ethptr, (byte *)arg1);
			break;

		/* Remove a multicast address */

		case ETH_CTRL_REMOVE_MCAST:
			retval = ethmcast_remove(ethptr, (byte *)arg1);
			break;

		default:
			return SYSERR;
	}

	return retval;
}
