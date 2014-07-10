/* mark.h - unmarked */

#ifndef	MM_MAX_MARKS
#define	MM_MAX_MARKS	20	/* maximum number of marked locations	*/
#endif

extern	uint32	*(marks[]);
extern	uint32	marked;
extern	sid32	mkmutex;
typedef	uint32	marker[1];	/* Because marker is declared to be an	*/
				/* array, it it unnecessary to use	*/
				/* an ampersand to obtain the address	*/

#define	unmarked(L)		(L[0]<0 || L[0]>=marked || marks[L[0]]!=L)
