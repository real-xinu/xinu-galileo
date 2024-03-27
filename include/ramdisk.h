/* ramdisk.h - definitions for a ram disk (for testing) */

/* Ram disk block size */

#define	RM_BLKSIZ	512		/* Block size			*/
#define	RM_BLKS		200		/* Number of blocks		*/

struct	ramdisk	{
	char	disk[RM_BLKSIZ * RM_BLKS];
	};

extern	struct	ramdisk	Ram;
