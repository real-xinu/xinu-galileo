/* delay.h - DELAY */

/*
 * Delay units are in microseconds.
 */
#define	DELAY(n)                                	\
{                                               	\
	extern	int cpudelay;				\
	register int i;					\
	register long N = (((n)<<4) >> cpudelay);	\
							\
	for (i=0;i<=4;i++) {				\
		N = (((n) << 4) >> cpudelay);		\
		while (--N > 0) ;			\
	}						\
}

/*
 * Delay units are in milli-seconds.
 */
#define	MDELAY(n)					\
{							\
	register long i;				\
							\
	for (i=n;i>0;i--) {				\
		DELAY(1000);				\
	}						\
}
