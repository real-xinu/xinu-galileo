/* name.h */

/* Constants that define the namespace mapping table sizes */

#define	NM_PRELEN	64		/* max size of a prefix string	*/
#define	NM_REPLLEN	96		/* maximum size of a replacement*/
#define	NM_MAXLEN	256		/* maximum size of a file name	*/
#define	NNAMES		40		/* number of prefix definitions	*/

/* Definition of the name prefix table that defines all name mappings */

struct	nmentry	{			/* definition of prefix table	*/
	char	nprefix[NM_PRELEN];	/* null-terminated prefix	*/
	char	nreplace[NM_REPLLEN];	/* null-terminated replacement	*/
	did32	ndevice;		/* device descriptor for prefix	*/
};

extern	struct	nmentry	nametab[];	/* table of name mappings	*/
extern	int32	nnames;			/* num. of entries allocated	*/
