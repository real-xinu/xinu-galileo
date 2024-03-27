/* bufpool.h */

#ifndef	NBPOOLS
#define	NBPOOLS	20		/* Maximum number of buffer pools	*/
#endif

#ifndef	BP_MAXB
#define	BP_MAXB	8192		/* Maximum buffer size in bytes		*/
#endif

#define	BP_MINB	8		/* Minimum buffer size in bytes		*/
#ifndef	BP_MAXN
#define	BP_MAXN	2048		/* Maximum number of buffers in a pool	*/
#endif

struct	bpentry	{		/* Description of a single buffer pool	*/
	struct	bpentry *bpnext;/* Pointer to next free buffer		*/
	sid32	bpsem;		/* Semaphore that counts buffers	*/
				/*    currently available in the pool	*/
	};

extern	struct	bpentry buftab[];/* Buffer pool table			*/
extern	bpid32	nbpools;	/* Current number of allocated pools	*/
