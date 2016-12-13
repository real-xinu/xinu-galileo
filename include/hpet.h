/* hpet.h  -  HPET related definitions */

#define	HPET_GC_OE	0x00000001	/* Overall Enable		*/
#define	GPET_GC_LRE	0x00000002	/* Legacy Routing Enable	*/

#define	HPET_GIS_T0	0x00000001	/* Timer0 Interrupt status	*/

#define	HPET_TXCC_IT	0x00000002	/* Intr. type level (edge)	*/
#define	HPET_TXCC_IE	0x00000004	/* Interrupt enable		*/
#define	HPET_TXCC_TYP	0x00000008	/* timer type periodic		*/
#define	HPET_TXCC_TVS	0x00000040	/* Timer Value Set		*/

/* Structure of HPET control and status registers */

struct	hpet_csreg {
	uint32	gcid_l;	/* General Capab. and ID (lower)*/
	uint32	gcid_u;	/* General Capab. and ID (upper)*/
	uint32	res1[2];
	uint32	gc;	/* General Configuration	*/
	uint32	res2[3];
	uint32	gis;	/* General Interrupt Status	*/
	uint32	res3[51];
	uint32	mcv_l;	/* Main Counter Value (lower)	*/
	uint32	mcv_u;	/* Main Counter Value (upper)	*/
	uint32	res4[2];
	uint32	t0cc_l;	/* Timer0 Config and Capab. (l)	*/
	uint32	t0cc_u;	/* Timer0 Config and Capab. (u)	*/
	uint32	t0cv_l;	/* Timer0 Comparator (lower)	*/
	uint32	t0cv_u;	/* Timer0 Comparator (upper)	*/
};

extern	volatile struct	hpet_csreg *hpet;

#define	HPET_BASE_ADDR	0xFED00000
