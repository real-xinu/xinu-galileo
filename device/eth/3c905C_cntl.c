/* 3c905C_cntl.c - _3c905C_cntl */

#include <xinu.h>

/*------------------------------------------------------------------------
 * _3c905C_cntl - implement control function for a 3c905C device
 *------------------------------------------------------------------------
 */
devcall	_3c905C_cntl(
	struct 	ether *ethptr, 		/* entry in device switch table */
	int32	func,			/* control function		*/
	int32	arg1,			/* argument 1, if needed	*/
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
