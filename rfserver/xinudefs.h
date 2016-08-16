/* kernel.h */

/* General type declarations used throughout Xinu */

typedef	unsigned char	byte;
typedef	int		int32;
typedef	short		int16;
typedef	unsigned int	uint32;
typedef	unsigned short	uint16;
typedef	int		sid32;
typedef	int		devcall;
typedef	char		bool8;

/* Boolean constants */

#define FALSE	0		/* Boolean False			*/
#define TRUE	1		/* Boolean True				*/

/* Null pointer, character, and string definintions */

#undef NULL

#define NULL	0		/* null pointer for linked lists	*/
#define NULLCH	'\0'		/* null character			*/
#define	NULLSTR	""		/* null string				*/

/* Universal return constants */

#define OK	( 1)		/* normal system call return		*/
#define	SYSERR	(-1)		/* system call failed			*/
