/* rand.c - srand, rand */

static unsigned long randx = 1;

/*------------------------------------------------------------------------
 *  srand  -  Sets the random seed.
 *------------------------------------------------------------------------
 */
void		srand(
			  unsigned long		x			/* random seed				*/
			)
{
    randx = x;
}

/*------------------------------------------------------------------------
 *  rand  -  Generates a random long.
 *------------------------------------------------------------------------
 */
unsigned long		rand(
					  void
					)
{
    return (((randx = randx * 1103515245 + 12345) >> 16) & 077777);
}
