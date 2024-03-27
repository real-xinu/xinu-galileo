/* kernel.h */

/* General type declarations used throughout the kernel */

typedef	unsigned char	byte;
typedef	unsigned char	uint8;
typedef	int		int32;
typedef	short		int16;
typedef	unsigned int	uint32;
typedef	unsigned short	uint16;
typedef	unsigned long long uint64;

/* Xinu-specific types */

typedef	int32	sid32;		/* Semaphore ID				*/
typedef	int16	qid16;		/* Queue ID				*/
typedef	int32	pid32;		/* Process ID				*/
typedef	int32	did32;		/* Device ID				*/
typedef	int16	pri16;		/* Process priority			*/
#define	MAXPRIO	0x7fff		/* Maximum possible priority		*/
typedef	uint32	umsg32;		/* Message passed among processes	*/
typedef	int32	bpid32;		/* Buffer pool ID			*/
typedef	byte	bool8;		/* Boolean type				*/
typedef	uint32	intmask;	/* Saved interrupt mask			*/
typedef	int32	ibid32;		/* Index block ID (used in file system)	*/
typedef	int32	dbid32;		/* Data block ID (used in file system)	*/
typedef	int32	uid32;		/* ID for UDP table descriptor		*/

/* Function declaration return types */

typedef int32	syscall;	/* System call declaration		*/
typedef int32	devcall;	/* Device call declaration		*/
typedef int32	shellcmd;	/* Shell command declaration		*/
typedef int32	process;	/* Top-level function of a process	*/
typedef	void	interrupt;	/* Interrupt function			*/
typedef	int32	status;		/* Returned status value (OK/SYSERR)	*/

#define local	static		/* Local procedure or variable declar.	*/

/* Boolean constants */

#define FALSE	0		/* Boolean False			*/
#define TRUE	1		/* Boolean True				*/

/* Null pointer, character, and string definintions */

#define NULL	0		/* Null pointer for linked lists	*/
#define NULLCH	'\0'		/* Null character			*/
#define	NULLSTR	""		/* Null string				*/

/* Universal return constants */

#define OK	( 1)		/* Normal system call return		*/
#define	SYSERR	(-1)		/* System call failed			*/
#define	EOF	(-2)		/* End-of-file (usually from read)	*/
#define	TIMEOUT	(-3)		/* System call timed out		*/

extern	qid16	readylist;	/* Global ID for list of ready processes*/

#define	MINSTK	400		/* Minimum stack size in bytes		*/

#define	CONTEXT	64		/* Bytes in a function call context on	*/
				/* The run-time stack			*/
#define	QUANTUM	2		/* Time slice in milliseconds		*/

/* Size of the stack for the null process */

#define	NULLSTK		8192	/* Stack  size for null process		*/

/* Prototypes of I/O functions used throughout the kernel */

syscall	kprintf(char *fmt, ...);
syscall	kputc(byte);
syscall	kgetc(void);
