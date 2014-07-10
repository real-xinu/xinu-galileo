/* e1000e_cmd.c  -  e1000e_irq_enable, e1000e_irq_disable */

#include <xinu.h>

/*------------------------------------------------------------------------
 * e1000e_irq_disable - Mask off interrupt generation on the NIC
 *------------------------------------------------------------------------
 */
void e1000e_irq_disable(
	struct 	ether *ethptr
	)
{
	e1000e_io_writel(IMC, ~0);

	e1000e_io_flush();
}

/*------------------------------------------------------------------------
 * e1000e_irq_enable - Enable default interrupt generation settings
 *------------------------------------------------------------------------
 */
void e1000e_irq_enable(
	struct 	ether *ethptr
	)
{
	e1000e_io_writel(IMS, IMS_ENABLE_MASK);

	e1000e_io_flush();
}
