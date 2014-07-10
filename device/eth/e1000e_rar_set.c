/* e10002_rar_set.c - e10002_rar_set */

#include <xinu.h>

/*************************************************************************
 * Store an Ethernet address in a Recbeive Address Register
 *************************************************************************
 */
void	e1000e_rar_set(
	  struct ether *ethptr,		/* ptr to Ethernet control block*/
	  byte   *addr,			/* ptr to Ethernet address	*/
	  uint32 index			/* index of RAR to use		*/
	)
{
	uint32	rar_low;		/* value for the "low" register	*/
	uint32	rar_high;		/* value for the "high" regist.	*/

	/* The Ethernet address is stored in an array in normal byte	*/
	/*	order, but the hardware requires us to divide the six	*/
	/*	bytes into a "high" and "low" register of 32 bits each	*/
	/*	Each of the two values must be in little-endian order.	*/
	/*	So an address aa:bb:cc:dd:ee:ff is set by placing	*/
	/*	dd:cc:bb:aa in the low register and 00:00:ee:ff in the	*/
	/*	high register.  The high register also has E1000_RAH_AV	*/
	/*	in the upper bytes.					*/

	rar_low  = *((uint32 *)addr);
	rar_high = (( *((uint16 *)(addr+4)) & 0x0000ffff) | E1000_RAH_AV);

	e1000e_io_writel(RAL(index), rar_low);
	e1000e_io_flush();
	e1000e_io_writel(RAH(index), rar_high);
	e1000e_io_flush();
	return;
}
