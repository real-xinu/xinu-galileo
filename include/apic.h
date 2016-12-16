/* apic.h - APIC related definitions */

/* Structure of APIC registers */

struct	lapic_csreg {
	uint32	res1[8];
	uint32	lapic_id;	/* LAPIC ID		*/
	uint32	res2[3];
	uint32	lapic_ver;	/* LAPIC version	*/
	uint32	res3[19];
	uint32	tpr;		/* Task Proirity	*/
	uint32	res4[3];
	uint32	apr;		/* Arbitration Priority	*/
	uint32	res5[3];
	uint32	ppr;		/* Processor Priority	*/
	uint32	res6[3];
	uint32	eoi;		/* End of Interrupt	*/
	uint32	res7[3];
	uint32	rrd;		/* Remote Read		*/
	uint32	res8[3];
	uint32	ldr;		/* Local Destination	*/
	uint32	res9[3];
	uint32	dfr;		/* Destination Format	*/
	uint32	res10[3];
	uint32	sivr;		/* Spurious Intr Vector	*/
	uint32	res11[3];
	struct	{
	  uint32 isr;
	  uint32 res[3];
	}	isr[8];		/* In-Service Registers	*/
	struct	{
	  uint32 tmr;
	  uint32 res[3];
	}	tmr[8];		/* Trigger Mode Regs	*/
	struct	{
	  uint32 irr;
	  uint32 res[3];
	}	irr[8];		/* Intr. Request Regs	*/
};

extern	volatile struct lapic_csreg *lapic;

#define	LAPIC_BASE_ADDR	0xFEE00000

#define	IOAPIC_IDX_ADDR	0xFEC00000
#define	IOAPIC_WIN_ADDR	0xFEC00010
