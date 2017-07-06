/* memcpy.c - memcpy */

/*------------------------------------------------------------------------
 *  memcpy  -  Copy a block of memory from src to dst, and return a
 *			  pointer to the destination
 *------------------------------------------------------------------------
 */
void	*memcpy(
	  void		*s,	/* Destination address			*/
	  const void	*ct,	/* source address			*/
	  int		n	/* number of bytes to copy		*/
	)
{
	if(n <= 0) {
		return s;
	}

	asm volatile (
			/* Source address is loaded in esi */
			/* Dest address is loaded in edi   */
			/* Count is loaded in ecx	   */
			"rep	movsb\n\t"
			:				/* Output	*/
			: "S" (ct), "D" (s), "c" (n)	/* Input	*/
			:				/* Clobber	*/
		     );
	return s;
}
