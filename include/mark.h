/* mark.h - notmarked */

#define	MAXMARK	20		/* Maximum number of marked locations	*/

extern	int32	*(marks[]);
extern	int32	nmarks;
typedef	int32	memmark[1];	/* Declare a memory mark to be an array	*/
				/*   so user can reference the name	*/
				/*   without a leading &		*/
